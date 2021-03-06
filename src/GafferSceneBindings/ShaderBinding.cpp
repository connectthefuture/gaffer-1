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

#include "boost/python.hpp"

#include "GafferBindings/DependencyNodeBinding.h"

#include "GafferScene/Shader.h"
#include "GafferScene/ShaderSwitch.h"

#include "GafferSceneBindings/ShaderBinding.h"

using namespace boost::python;

using namespace Gaffer;
using namespace GafferBindings;
using namespace GafferScene;

namespace
{

IECore::CompoundObjectPtr attributes( const Shader &s, bool copy=true )
{
	IECore::ConstCompoundObjectPtr o = s.attributes();
	if( copy )
	{
		return o->copy();
	}
	else
	{
		return boost::const_pointer_cast<IECore::CompoundObject>( o );
	}
}

IECore::ObjectVectorPtr state( const Shader &s, bool copy=true )
{
	IECore::ConstObjectVectorPtr o = s.state();
	if( copy )
	{
		return o->copy();
	}
	else
	{
		return boost::const_pointer_cast<IECore::ObjectVector>( o );
	}
}

} // namespace

void GafferSceneBindings::bindShader()
{

	GafferBindings::DependencyNodeClass<Shader>()
		.def( "attributesHash", (IECore::MurmurHash (Shader::*)() const )&Shader::attributesHash )
		.def( "attributesHash", (void (Shader::*)( IECore::MurmurHash &h ) const )&Shader::attributesHash )
		.def( "attributes", &attributes, ( boost::python::arg_( "_copy" ) = true ) )
		.def( "stateHash", (IECore::MurmurHash (Shader::*)() const )&Shader::stateHash )
		.def( "stateHash", (void (Shader::*)( IECore::MurmurHash &h ) const )&Shader::stateHash )
		.def( "state", &state, ( boost::python::arg_( "_copy" ) = true ) )
	;

	GafferBindings::DependencyNodeClass<ShaderSwitch>();

}
