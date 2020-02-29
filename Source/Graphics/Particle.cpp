#include "PrecompiledHeader.h"
#include "Particle.h"

namespace Delta3D::Graphics
{
Particle::Particle( Effekseer::Effect* effect_ ) : 
	GraphicsImpl(), 
	Core::TimerImpl(), 
	effect( effect_ ), 
	handle( -1 ),
	currentTime( 0.0f ), 
	duration( 0.0f ), 
	finished( false )
{
}

Particle::~Particle()
{
	Core::Timer::DeleteTimer( this );
}

void Particle::Update( float timeElapsed )
{
	//Particle has duration specified?
	if( duration > 0.0f )
	{
		//Isn't paused?
		if( !graphics->GetEffectManager()->GetPaused( handle ) )
		{
			currentTime += timeElapsed;

			//Stop Particle
			if( currentTime >= duration )
			{
				currentTime = 0.0f;
				graphics->GetEffectManager()->StopEffect( handle );
			}
		}
	}
}

void Particle::Start( const Math::Vector3& position, float timeDuration )
{
	handle = graphics->GetEffectManager()->Play( effect, position.x, position.y, position.z );

	currentTime = 0.0f;
	duration = timeDuration;
	finished = false;
}

void Particle::Stop()
{
	graphics->GetEffectManager()->StopEffect( handle );
}

void Particle::Pause()
{
	graphics->GetEffectManager()->SetPaused( handle, !graphics->GetEffectManager()->GetPaused( handle ) );
}

void Particle::SetPosition( const Math::Vector3& position )
{
	graphics->GetEffectManager()->SetLocation( handle, position.x, position.y, position.z );
	graphics->GetEffectManager()->UpdateHandle( handle, 0.0f );
}

void Particle::SetScaling( const Math::Vector3& scaling )
{
	graphics->GetEffectManager()->SetScale( handle, scaling.x, scaling.y, scaling.z );
	graphics->GetEffectManager()->UpdateHandle( handle, 0.0f );
}

void Particle::SetRotation( const Math::Vector3& rotation )
{
	graphics->GetEffectManager()->SetRotation( handle, rotation.x, rotation.y, rotation.z );
	graphics->GetEffectManager()->UpdateHandle( handle, 0.0f );
}

void Particle::SetTargetPosition( const Math::Vector3& targetPosition )
{
	graphics->GetEffectManager()->SetTargetLocation( handle, targetPosition.x, targetPosition.y, targetPosition.z );
}

void Particle::SetEndCallback( std::function<void( int handle )> callback )
{
	//Deprecated graphics->GetEffectManager()->SetRemovingCallback( handle, std::bind( &Particle::OnEndCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 ) );
}

void Particle::SetSpeed( float speed )
{
	graphics->GetEffectManager()->SetSpeed( handle, speed );
}

void Particle::OnEndCallback( Effekseer::Manager* manager, Effekseer::Handle handle, bool isRemovingManager )
{
	finished = true;

	if( endCallback )
		endCallback( handle );
}

ParticleFactory::ParticleFactory( Graphics* graphics_ ) : graphics( graphics_ )
{
}

ParticleFactory::~ParticleFactory()
{
	for( auto& effect : cache )
	{
		if( effect.second )
		{
			effect.second->Release();
			effect.second = nullptr;
		}
	}

	cache.clear();
}

void ParticleFactory::OnLostDevice()
{
	for( auto& effect : cache )
		effect.second->UnloadResources();
}

void ParticleFactory::OnResetDevice()
{
	for( auto& effect : cache )
		effect.second->ReloadResources();
}

void ParticleFactory::Start( const std::string& filePath, const Math::Vector3& position )
{
	auto effect = CreateEffect( filePath );

	if( effect )
	{
		//Put Effect on Cache
		cache[filePath] = effect;

		graphics->GetEffectManager()->Play( effect, position.x, position.y, position.z );
	}
}

std::shared_ptr<Particle> ParticleFactory::Create( const std::string& filePath )
{
	auto effect = CreateEffect( filePath );

	if( effect )
	{
		//Create Particle Object
		auto particle = std::make_shared<Particle>( effect );

		//Put Effect on Cache
		cache[filePath] = effect;

		return particle;
	}

	return nullptr;
}

Effekseer::Effect* ParticleFactory::CreateEffect( const std::string& filePath )
{
	auto it = cache.find( filePath );

	if( it != cache.end() )
		return (*it).second;

	std::wstring filePathUTF16;
	const int chars_required = MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, filePath.c_str(), filePath.size(), NULL, 0 );
	filePathUTF16.resize( chars_required );
	MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, filePath.c_str(), filePath.size(), &filePathUTF16[0], chars_required );

	return Effekseer::Effect::Create( graphics->GetEffectManager(), (const EFK_CHAR*)filePathUTF16.c_str() );
}
}