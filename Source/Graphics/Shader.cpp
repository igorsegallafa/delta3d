#include "PrecompiledHeader.h"
#include "Shader.h"

namespace Delta3D::Graphics
{

const std::map<std::string,int> DefinesValue = { {"SELFILLUMINATIONMAP", 1 << 0 },
												 {"SKINNED", 1 << 1 }, 
												 {"VERTEXCOLOR", 1 << 2 },
												 {"_PS_3_0", 1 << 3 },
												 {"REFLECTION", 1 << 4 },
												 {"SHADOWS", 1 << 5 },
											   };

Shader::Shader( LPD3DXEFFECT effect_, const std::string& filePath_ ) : effect( effect_ ), filePath( filePath_ )
{
	if( effect )
		effect->AddRef();
}

Shader::~Shader()
{
	if( effect )
	{
		effect->Release();
		effect = nullptr;
	}
}

void Shader::OnLostDevice()
{
	effect->OnLostDevice();
}

void Shader::OnResetDevice()
{
	effect->OnResetDevice();
}

bool Shader::SetTechnique( const std::string& techniqueName )
{
	return SUCCEEDED( effect->SetTechnique( techniqueName.c_str() ) );
}

int Shader::Begin()
{
	return SUCCEEDED( effect->Begin( &numPasses, D3DXFX_DONOTSAVESAMPLERSTATE ) ) ? numPasses : -1;
}

void Shader::SetData( const std::string& variable, const char* data, unsigned int size )
{
	HRESULT hr;
	if( FAILED( hr = effect->SetRawValue( variable.c_str(), data, 0, size ) ) )
		DELTA3D_LOGERROR( "Could not set Data to Effect (%s) [%08X]", variable.c_str(), hr );
}

void Shader::SetMatrix( const std::string& variable, const D3DXMATRIX& matrix )
{
	HRESULT hr;
	if( FAILED( hr = effect->SetMatrix( variable.c_str(), &matrix ) ) )
		DELTA3D_LOGERROR( "Could not set Matrix to Effect (%s) [%08X]", variable.c_str(), hr );
}

void Shader::SetMatrixArray( const std::string& variable, const D3DXMATRIX* matrix, unsigned int count )
{
	HRESULT hr;
	if( FAILED( hr = effect->SetMatrixArray( variable.c_str(), matrix, count ) ) )
		DELTA3D_LOGERROR( "Could not set Matrix Array to Effect (%s)(%d) [%08X]", variable.c_str(), count, hr );
}

void Shader::SetInt( const std::string& variable, int value )
{
	HRESULT hr;
	if( FAILED( hr = effect->SetInt( variable.c_str(), value ) ) )
		DELTA3D_LOGERROR( "Could not set Integer to Effect (%s) [%08X]", variable.c_str(), hr );
}

void Shader::SetIntArray( const std::string& variable, const int* value, unsigned int count )
{
	HRESULT hr;
	if( FAILED( hr = effect->SetIntArray( variable.c_str(), value, count ) ) )
		DELTA3D_LOGERROR( "Could not set Integer Array to Effect (%s)(%d) [%08X]", variable.c_str(), count, hr );
}

void Shader::SetFloat( const std::string& variable, float value )
{
	HRESULT hr;
	if( FAILED( hr = effect->SetFloat( variable.c_str(), value ) ) )
		DELTA3D_LOGERROR( "Could not set Float to Effect (%s) [%08X]", variable.c_str(), hr );
}

void Shader::SetFloatArray( const std::string& variable, const float* value, unsigned int count )
{
	HRESULT hr;
	if( FAILED( hr = effect->SetFloatArray( variable.c_str(), value, count ) ) )
		DELTA3D_LOGERROR( "Could not set Float Array to Effect (%s)(%d) [%08X]", variable.c_str(), count, hr );
}

void Shader::SetBool( const std::string& variable, BOOL value )
{
	HRESULT hr;
	if( FAILED( hr = effect->SetBool( variable.c_str(), value ) ) )
		DELTA3D_LOGERROR( "Could not set Boolean to Effect (%s) [%08X]", variable.c_str(), hr );
}

void Shader::SetBoolArray( const std::string& variable, const BOOL* value, unsigned int count )
{
	HRESULT hr;
	if( FAILED( hr = effect->SetBoolArray( variable.c_str(), value, count ) ) )
		DELTA3D_LOGERROR( "Could not set Boolean Array to Effect (%s)(%d) [%08X]", variable.c_str(), count, hr );
}

void Shader::SetTexture( const std::string& variable, std::shared_ptr<Texture> texture )
{
	D3DXHANDLE textureHandle = effect->GetParameterByName( 0, variable.c_str() );

	if( texture )
	{
		HRESULT hr;

		if( FAILED( hr = effect->SetTexture( textureHandle, texture->Get() ) ) )
			DELTA3D_LOGERROR( "Could not set Set Texture to Effect (%s) [%08X]", variable.c_str(), hr );
	}
	else
		DELTA3D_LOGERROR( "Could not Get parameter from Effect (%s)", variable.c_str() );
}

void Shader::SetTexture( const std::string& variable, IDirect3DTexture9* texture )
{
	D3DXHANDLE textureHandle = effect->GetParameterByName( 0, variable.c_str() );

	if( texture )
	{
		HRESULT hr;

		if( FAILED( hr = effect->SetTexture( textureHandle, texture ) ) )
			DELTA3D_LOGERROR( "Could not set Set Texture to Effect (%s) [%08X]", variable.c_str(), hr );
	}
	else
		DELTA3D_LOGERROR( "Could not Get parameter from Effect (%s)", variable.c_str() );
}

bool Shader::BeginPass( unsigned int pass )
{
	HRESULT hr;

	if( FAILED( hr = effect->BeginPass( pass ) ) )
	{
		DELTA3D_LOGERROR( "Error on Effect Begin Pass (%s)(%d) [%08X]", filePath.c_str(), pass, hr );
		return false;
	}

	return true;
}

void Shader::CommitChanges()
{
	effect->CommitChanges();
}

void Shader::EndPass()
{
	effect->EndPass();
}

void Shader::End()
{
	effect->End();
}

void Shader::Renew( ID3DXEffect* effect_ )
{
	if( effect )
	{
		effect->Release();
		effect = effect_;
		effect->AddRef();
	}
}

ShaderFactory::ShaderFactory( Graphics* graphics_ ) : graphics( graphics_ )
{
}

void ShaderFactory::OnLostDevice()
{
	for( const auto& effect : cache )
		effect->OnLostDevice();
}

void ShaderFactory::OnResetDevice()
{
	for( const auto& effect : cache )
		effect->OnResetDevice();
}

void ShaderFactory::Reload()
{
	//Reload All Effects from Cache
	for( auto& effect : cache )
	{
		//Create Effect
		ID3DXEffect* effectd3d = CreateShader( effect->FilePath(), effect->Defines() );

		//Renew Effect
		effect->Renew( effectd3d );
		effectd3d->Release();
	}
}

std::shared_ptr<Shader> ShaderFactory::Create( const std::string& filePath, std::vector<ShaderDefine> defines )
{
	//Defines List Tail
	if( !defines.empty() )
		defines.push_back( ShaderDefine{ nullptr, nullptr } );

	//Find effect on Cache
	for( const auto& effect : cache )
		if( _strcmpi( effect->FilePath().c_str(), filePath.c_str() ) == 0 && effect->Defines() == defines )
			return effect;

	//Create Effect
	ID3DXEffect* effectd3d = CreateShader( filePath, defines );

	if( effectd3d )
	{
		auto effect = std::make_shared<Shader>( effectd3d, filePath );

		effect->effectDefines = defines;

		//Put it on Cache
		cache.push_back( effect );

		//Release our Reference
		effectd3d->Release();

		return effect;
	}

	return nullptr;
}

ID3DXEffect* ShaderFactory::CreateShader( const std::string& filePath, std::vector<ShaderDefine> defines )
{
	DWORD flags = 0;

#ifdef DEBUG
	flags |= D3DXSHADER_DEBUG;
#endif

	ID3DXEffect* effectd3d;
	ID3DXBuffer* errorBuffer;

	//Compiled Effect?
	if( filesystem::exists( filePath + "c" ) )
	{
		std::string compiledFilePath = filePath.substr( 0, filePath.find_last_of(".") );

		unsigned int definesValue = 0;

		for( const auto& define : defines )
		{
			if( define.name )
			{
				auto it = DefinesValue.find( define.name );

				if( it != DefinesValue.end() )
					definesValue |= ( *it ).second;
			}
		}

		if( definesValue > 0 )
			compiledFilePath += std::to_string( definesValue );

		compiledFilePath += ".fxc";

		//Create from Compiled Effect
		if( FAILED( D3DXCreateEffectFromFile( graphics->GetDevice(), compiledFilePath.c_str(), nullptr, nullptr, flags, nullptr, &effectd3d, &errorBuffer ) ) )
		{
			DELTA3D_LOGERROR( "Could not create Compiled Effect from File (%s): %s", filePath.c_str(), errorBuffer ? errorBuffer->GetBufferPointer() : "Unknown Reason" );

			if( errorBuffer )
				errorBuffer->Release();

			return nullptr;
		}
	}
	else
	{
		//Create Normal Effect
		if( FAILED( D3DXCreateEffectFromFile( graphics->GetDevice(), filePath.c_str(), defines.empty() ? nullptr : (D3DXMACRO*)defines.data(), nullptr, flags, nullptr, &effectd3d, &errorBuffer ) ) )
		{
			DELTA3D_LOGERROR( "Could not create Effect from File (%s): %s", filePath.c_str(), errorBuffer ? errorBuffer->GetBufferPointer() : "Unknown Reason" );

			if( errorBuffer )
				errorBuffer->Release();

			return nullptr;
		}
	}

	//Release Error Buffer Object
	if( errorBuffer )
		errorBuffer->Release();

	return effectd3d;
}

}