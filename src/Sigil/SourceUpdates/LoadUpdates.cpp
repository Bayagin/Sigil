/************************************************************************
**
**  Copyright (C) 2009  Strahinja Markovic
**
**  This file is part of Sigil.
**
**  Sigil is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  Sigil is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Sigil.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/

#include <stdafx.h>
#include "LoadUpdates.h"
#include <QDomDocument>
#include "../BookManipulation/XHTMLDoc.h"

static const QStringList PATH_TAGS       = QStringList() << "link" << "a" << "img" << "image";
static const QStringList PATH_ATTRIBUTES = QStringList() << "href" << "src";


LoadUpdates::LoadUpdates( const QString &source, const QHash< QString, QString > &updates )
    : m_Source( source ) 
{
    m_HTMLUpdates = updates;
    QList< QString > keys = m_HTMLUpdates.keys();
    int num_keys = keys.count();

    for ( int i = 0; i < num_keys; ++i )
    {
        QString key_path = keys.at( i );
        QString extension = QFileInfo( key_path ).suffix().toLower();

        // Font file updates are CSS updates, not HTML updates
        if ( extension == "ttf" || extension == "otf" )
        {
            m_CSSUpdates[ key_path ] = m_HTMLUpdates.value( key_path );
            m_HTMLUpdates.remove( key_path );
        }
    }
}


QString LoadUpdates::operator()()
{
    UpdateHTMLReferences();
    UpdateCSSReferences();

    return m_Source;
}


void LoadUpdates::UpdateHTMLReferences()
{
    QDomDocument document;
    document.setContent( m_Source );

    QList< QDomNode > nodes = XHTMLDoc::GetTagMatchingChildren( document.documentElement(), PATH_TAGS );

    int node_count = nodes.count();

    for ( int j = 0; j < node_count; ++j )
    {
        m_NodeUpdateSynchronizer.addFuture(
            QtConcurrent::run( this, &LoadUpdates::UpdateReferenceInNode, nodes.at( j ) ) );
    }

    // We wait until all the nodes are updated
    m_NodeUpdateSynchronizer.waitForFinished();

    m_Source = XHTMLDoc::GetQDomNodeAsString( document );
}


void LoadUpdates::UpdateReferenceInNode( QDomNode node )
{
    QDomNamedNodeMap attributes = node.attributes();
    int num_attributes = attributes.count();

    for ( int i = 0; i < num_attributes; ++i )
    {
        QDomAttr attribute = attributes.item( i ).toAttr();

        if ( !attribute.isNull() && 
             PATH_ATTRIBUTES.contains( XHTMLDoc::GetAttributeName( attribute ), Qt::CaseInsensitive ) )
        {
            QList< QString > keys = m_HTMLUpdates.keys();
            int num_keys = keys.count();

            for ( int j = 0; j < num_keys; ++j )
            {
                QString key_path  = keys.at( j );
                QString filename  = QFileInfo( key_path ).fileName();
                QString atr_value = QUrl::fromPercentEncoding( attribute.value().toUtf8() );

                if ( atr_value == filename || atr_value.endsWith( "/" + filename ) )
                {
                    QByteArray encoded_url = QUrl::toPercentEncoding( m_HTMLUpdates[ key_path ], QByteArray( "/" ) );

                    attribute.setValue( QString::fromUtf8( encoded_url.constData(), encoded_url.count() ) );
                }
            }            
        }
    }
}


void LoadUpdates::UpdateCSSReferences()
{
    QList< QString > keys = m_CSSUpdates.keys();
    int num_keys = keys.count();

    for ( int i = 0; i < num_keys; ++i )
    {
        QString key_path = keys.at( i );
        QString filename = QFileInfo( key_path ).fileName();

        QRegExp reference = QRegExp( "src:\\s*\\w+\\([\"']*([^\\)]*/" + QRegExp::escape( filename ) + "|"
            + QRegExp::escape( filename ) + ")[\"']*\\)" );

        int index = -1;

        while ( true )
        {
            int newindex = m_Source.indexOf( reference );

            // We need to make sure we don't end up
            // replacing the same thing over and over again
            if ( ( index == newindex ) || ( newindex == -1 ) )

                break;

            m_Source.replace( reference.cap( 1 ), m_CSSUpdates.value( key_path ) );

            index = newindex;
        }
    }  
}

