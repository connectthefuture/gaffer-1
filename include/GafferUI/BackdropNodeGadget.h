//////////////////////////////////////////////////////////////////////////
//  
//  Copyright (c) 2013, Image Engine Design Inc. All rights reserved.
//  
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//  
//      * Redistributions of source code must retain the above
//        copyright notice, this list of conditions and the following
//        disclaimer.
//  
//      * Redistributions in binary form must reproduce the above
//        copyright notice, this list of conditions and the following
//        disclaimer in the documentation and/or other materials provided with
//        the distribution.
//  
//      * Neither the name of John Haddon nor the names of
//        any other contributors to this software may be used to endorse or
//        promote products derived from this software without specific prior
//        written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
//  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//  
//////////////////////////////////////////////////////////////////////////

#ifndef GAFFERUI_BACKDROPNODEGADGET_H
#define GAFFERUI_BACKDROPNODEGADGET_H

#include "Gaffer/Backdrop.h"
#include "Gaffer/BoxPlug.h"

#include "GafferUI/NodeGadget.h"

namespace GafferUI
{


class BackdropNodeGadget : public NodeGadget
{

	public :

		BackdropNodeGadget( Gaffer::NodePtr node );
		virtual ~BackdropNodeGadget();
		
		IE_CORE_DECLARERUNTIMETYPEDEXTENSION( GafferUI::BackdropNodeGadget, BackdropNodeGadgetTypeId, NodeGadget );
		
		virtual std::string getToolTip( const IECore::LineSegment3f &line ) const;

		/// Resizes the backdrop to frame the specified nodes.
		/// \undoable
		void frame( const std::vector<Gaffer::Node *> &nodes );
		/// Fills nodes with all the nodes currently enclosed by the backdrop.
		void framed( std::vector<Gaffer::Node *> &nodes ) const;

		Imath::Box3f bound() const;

	protected :

		virtual void doRender( const Style *style ) const;
		
	private :

		void plugDirtied( const Gaffer::Plug *plug );
		
		bool mouseMove( Gadget *gadget, const ButtonEvent &event );
		bool buttonPress( Gadget *gadget, const ButtonEvent &event );
		IECore::RunTimeTypedPtr dragBegin( Gadget *gadget, const DragDropEvent &event );
		bool dragEnter( Gadget *gadget, const DragDropEvent &event );
		bool dragMove( Gadget *gadget, const DragDropEvent &event );
		bool dragEnd( Gadget *gadget, const DragDropEvent &event );
		void leave( Gadget *gadget, const ButtonEvent &event );
	
		void selectionChanged( Gaffer::Set *set, IECore::RunTimeTyped *member );
	
		// The width in gadget coordinates that can be hovered and then dragged at the edge of frame.
		float hoverWidth() const;
		// -1 means the min in that direction, +1 means the max in that direction, 0
		// means not hovered in that direction.
		void hoveredEdges( const ButtonEvent &event, int &horizontal, int &vertical ) const;
		
		Gaffer::Box2fPlug *boundPlug();
		const Gaffer::Box2fPlug *boundPlug() const;
	
		bool m_hovered;
		int m_horizontalDragEdge;
		int m_verticalDragEdge;
		boost::signals::connection m_selectionAddedConnection;
		boost::signals::connection m_selectionRemovedConnection;
	
		static NodeGadgetTypeDescription<BackdropNodeGadget> g_nodeGadgetTypeDescription;
		
};

IE_CORE_DECLAREPTR( BackdropNodeGadget );

} // namespace GafferUI

#endif // GAFFERUI_BACKDROPNODEGADGET_H