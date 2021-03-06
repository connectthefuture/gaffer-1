//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2015, Image Engine Design Inc. All rights reserved.
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

#include "IECore/PointsPrimitive.h"

#include "Gaffer/StringPlug.h"

#include "GafferScene/PointsType.h"

using namespace IECore;
using namespace Gaffer;
using namespace GafferScene;

IE_CORE_DEFINERUNTIMETYPED( PointsType );

size_t PointsType::g_firstPlugIndex = 0;

PointsType::PointsType( const std::string &name )
	:	SceneElementProcessor( name )
{
	storeIndexOfNextChild( g_firstPlugIndex );
	addChild( new StringPlug( "type", Plug::In, "" ) );
}

PointsType::~PointsType()
{
}

Gaffer::StringPlug *PointsType::typePlug()
{
	return getChild<StringPlug>( g_firstPlugIndex );
}

const Gaffer::StringPlug *PointsType::typePlug() const
{
	return getChild<StringPlug>( g_firstPlugIndex );
}

void PointsType::affects( const Gaffer::Plug *input, AffectedPlugsContainer &outputs ) const
{
	SceneElementProcessor::affects( input, outputs );

	if( input == typePlug() )
	{
		outputs.push_back( outPlug()->objectPlug() );
	}
	else if( input == outPlug()->objectPlug() )
	{
		outputs.push_back( outPlug()->boundPlug() );
	}
}

bool PointsType::processesBound() const
{
	return true;
}

void PointsType::hashProcessedBound( const ScenePath &path, const Gaffer::Context *context, IECore::MurmurHash &h ) const
{
	hashProcessedObject( path, context, h );
}

Imath::Box3f PointsType::computeProcessedBound( const ScenePath &path, const Gaffer::Context *context, const Imath::Box3f &inputBound ) const
{
	ConstObjectPtr object = outPlug()->objectPlug()->getValue();
	if( const Primitive *primitive = runTimeCast<const Primitive>( object.get() ) )
	{
		return primitive->bound();
	}
	return inputBound;
}

bool PointsType::processesObject() const
{
	return true;
}

void PointsType::hashProcessedObject( const ScenePath &path, const Gaffer::Context *context, IECore::MurmurHash &h ) const
{
	typePlug()->hash( h );
}

IECore::ConstObjectPtr PointsType::computeProcessedObject( const ScenePath &path, const Gaffer::Context *context, IECore::ConstObjectPtr inputObject ) const
{
	const PointsPrimitive *inputPoints = runTimeCast<const PointsPrimitive>( inputObject.get() );
	if( !inputPoints )
	{
		return inputObject;
	}

	const std::string type = typePlug()->getValue();
	if( type == "" )
	{
		return inputObject;
	}

	if( const StringData *existingType = inputPoints->variableData<StringData>( "type" ) )
	{
		if( existingType->readable() == type )
		{
			return inputObject;
		}
	}

	PointsPrimitivePtr result = inputPoints->copy();
	result->variables["type"] = PrimitiveVariable( PrimitiveVariable::Constant, new StringData( type ) );

	return result;
}
