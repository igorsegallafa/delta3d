#pragma once

#include "Graphics.h"

#include "../Math/Vector3.h"
#include "../Core/TimerImpl.h"

namespace Delta3D::Graphics
{

class Particle : GraphicsImpl, public Core::TimerImpl
{
public:
	//! Default Constructor for Particle.
	Particle( Effekseer::Effect* effect_ );

	//! Deconstructor.
	~Particle();

	//! Update Particle.
	void Update( float timeElapsed );

	//! Spawn Particle.
	void Start( const Math::Vector3& position = Math::Vector3(), float timeDuration = 0.0f );

	//! Stop Particle.
	void Stop();

	//! Pause or Resume Particle.
	void Pause();

	//! Set Particle Position.
	void SetPosition( const Math::Vector3& position );

	//! Set Particle Scaling.
	void SetScaling( const Math::Vector3& scaling );

	//! Set Particle Rotation.
	void SetRotation( const Math::Vector3& rotation );

	//! Set Target Position.
	void SetTargetPosition( const Math::Vector3& targetPosition );

	//! Set End Particle Callback.
	void SetEndCallback( std::function<void( int handle )> callback );

	//! Set Particle Speed.
	void SetSpeed( float speed );

	//! Animation has finished
	bool HasFinished() const{ return finished; }

	//! Handle Getter.
	int GetHandle() const{ return handle; }
private:
	void OnEndCallback( Effekseer::Manager* manager, Effekseer::Handle handle, bool isRemovingManager );
private:
	Effekseer::Handle handle;	//!< Particle Handler
	Effekseer::Effect* effect;	//!< Effect Object

	std::function<void( int handle )> endCallback;	//!< Particle End Callback

	float currentTime;
	float duration;

	bool finished;
};

class ParticleFactory
{
public:
	//! Default Constructor for Particle Factory.
	ParticleFactory( Graphics* graphics_ );

	//! Deconstructor.
	~ParticleFactory();

	//! Device has been lost.
	void OnLostDevice();

	//! Device must be reseted.
	void OnResetDevice();

	//! Start Particle.
	void Start( const std::string& filePath, const Math::Vector3& position );

	//! Create Particle Object.
	std::shared_ptr<Particle> Create( const std::string& filePath );
private:
	Effekseer::Effect* CreateEffect( const std::string& filePath );
private:
	std::unordered_map<std::string, Effekseer::Effect*> cache;	//!< Cache of Effects

	Graphics* graphics;	//!< Graphics Pointer
};
}