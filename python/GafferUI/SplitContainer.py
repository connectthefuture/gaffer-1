##########################################################################
#  
#  Copyright (c) 2011, John Haddon. All rights reserved.
#  Copyright (c) 2011, Image Engine Design Inc. All rights reserved.
#  
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are
#  met:
#  
#      * Redistributions of source code must retain the above
#        copyright notice, this list of conditions and the following
#        disclaimer.
#  
#      * Redistributions in binary form must reproduce the above
#        copyright notice, this list of conditions and the following
#        disclaimer in the documentation and/or other materials provided with
#        the distribution.
#  
#      * Neither the name of John Haddon nor the names of
#        any other contributors to this software may be used to endorse or
#        promote products derived from this software without specific prior
#        written permission.
#  
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
#  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
#  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
#  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
#  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#  
##########################################################################

import IECore

import GafferUI

QtCore = GafferUI._qtImport( "QtCore" )
QtGui = GafferUI._qtImport( "QtGui" )

## \todo Support other list operations for child access
class SplitContainer( GafferUI.ContainerWidget ) :
	
	Orientation = IECore.Enum.create( "Vertical", "Horizontal" )
	
	def __init__( self, orientation=Orientation.Vertical ) :
		
		GafferUI.ContainerWidget.__init__( self, _Splitter() )
		
		self.__widgets = []
		self.__handleWidgets = {}
		self.__sizeAnimation = None
		
		self.setOrientation( orientation )
	
	def getOrientation( self ) :
	
		o = self._qtWidget().orientation()
		if o == QtCore.Qt.Horizontal :
			return self.Orientation.Horizontal
		else :
			return self.Orientation.Vertical
		
	def setOrientation( self, orientation ) :
	
		v = QtCore.Qt.Horizontal if orientation == self.Orientation.Horizontal else QtCore.Qt.Vertical
		self._qtWidget().setOrientation( QtCore.Qt.Orientation( v ) )
			
	def append( self, child ) :
			
		oldParent = child.parent()
		if oldParent is not None :
			oldParent.removeChild( child )
			
		self.__widgets.append( child )
		self._qtWidget().addWidget( child._qtWidget() )
	
	def remove( self, child ) :
		
		self.removeChild( child )
		
	def insert( self, index, child ) :
	
		assert( child not in self.__widgets )
		
		oldParent = child.parent()
		if oldParent :
			oldParent.removeChild( child )
			
		self._qtWidget().insertWidget( index,  child._qtWidget() )
		self.__widgets.insert( index, child )
	
	def index( self, child ) :
	
		return self.__widgets.index( child )
	
	def addChild( self, child ) :
	
		self.append( child )
			
	def removeChild( self, child ) :
	
		assert( child in self.__widgets )
		child._qtWidget().setParent( None )
		self.__widgets.remove( child )
	
	## Returns a list of actual pixel sizes for each of the children.
	# These do not include the space taken up by the handles.
	def getSizes( self ) :
	
		return self._qtWidget().sizes()
	
	## Sets the sizes of the children. Note that this will not change
	# the overall size of the SplitContainer - instead the sizes are
	# adjusted to take up all the space available. Therefore it is only
	# the relative differences in sizes which are important.
	# If animationDuration is non-zero then it specifies a period
	# in milliseconds over which to adjust the sizes.
	def setSizes( self, sizes, animationDuration=0 ) :
	
		assert( len( sizes ) == len( self ) )
		
		if self.getOrientation() == self.Orientation.Vertical :
			availableSize = self.size().y
		else :
			availableSize = self.size().x
			
		if len( self ) > 1 :
			handleSize = 0
			if self.getOrientation() == self.Orientation.Vertical :
				handleSize = self.handle( 0 ).size().y
			else :
				handleSize = self.handle( 0 ).size().x	
			availableSize -= (len( self ) - 1) * handleSize
				
		scaleFactor = availableSize / sum( sizes )
		sizes = [ scaleFactor * x for x in sizes ]
		
		if animationDuration == 0 :
			self._qtWidget().setSizes( sizes )
		else :
			animation = _SizeAnimation( self._qtWidget(), sizes )
			animation.setDuration( animationDuration )
			self.__sizeAnimation = animation
			animation.start()
			
	## If a size animation is currently in progress, then returns the
	# final sizes of the animation, otherwise returns None.
	def targetSizes( self ) :
	
		if self.__sizeAnimation is not None :
			if self.__sizeAnimation.state() == _SizeAnimation.Running :
				return self.__sizeAnimation.targetSizes()
		
		return None
	
	## Returns the handle to the right/bottom of the specified child index.
	# Note that you should not attempt to reparent the handles, and you will
	# be unable to access them after the SplitContainer itself has been destroyed.
	def handle( self, index ) :
	
		if index < 0 or index >= len( self ) - 1 :
			raise IndexError()
	
		qtHandle = self._qtWidget().handle( index + 1 )
		handle = self.__handleWidgets.get( qtHandle, None )
		if handle is None :
			handle = GafferUI.Widget( qtHandle )
			self.__handleWidgets[qtHandle] = handle
			
		return handle
	
	def __getitem__( self, index ) :
	
		return self.__widgets[index]
	
	def __delitem__( self, index ) :
	
		if isinstance( index, slice ) :
			indices = range( *(index.indices( len( self ) )) )
			toRemove = []
			for i in indices :
				toRemove.append( self[i] )
			for c in toRemove :
				self.removeChild( c )			
		else :
			self.removeChild( self[index] )
	
	def __len__( self ) :
		
		return len( self.__widgets )

# We inherit from QSplitter purely so that the handles can be created
# in Python rather than C++. This seems to help PyQt and PySide in tracking
# the lifetimes of the splitter and handles.
class _Splitter( QtGui.QSplitter ) :

	def __init__( self ) :
	
		QtGui.QSplitter.__init__( self )
		
	def createHandle( self ) :
			
		return QtGui.QSplitterHandle( self.orientation(), self )

class _SizeAnimation( QtCore.QVariantAnimation ) :

	def __init__( self, qSplitter, newSizes ) :
	
		QtCore.QVariantAnimation.__init__( self, None )
		
		self.__splitter = qSplitter
		self.sizes = zip( qSplitter.sizes(), newSizes )
		self.setStartValue( 0.0 )
		self.setEndValue( 1.0 )
		self.setEasingCurve( QtCore.QEasingCurve( QtCore.QEasingCurve.OutCubic ) )
	
	def targetSizes( self ) :
	
		return ( self.sizes[0][1], self.sizes[1][1] )
	
	def updateCurrentValue( self, value ) :

		value = GafferUI._Variant.fromVariant( value )
		sizes = [ x[0] + ( x[1] - x[0] ) * value for x in self.sizes ]
				
		self.__splitter.setSizes( sizes )
