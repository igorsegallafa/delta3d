#pragma once

#include "Vector2.h"
#include "Color.h"

#include "../Core/EventsImpl.h"
#include "../Core/TimerImpl.h"

#include "Easing.h"

namespace Delta3D::Math
{

template<typename T>
class ValueAnimation
{

struct KeyFrameAnimation
{
	float time;
	T value;
	std::function<T( float, T, T, float )> easing;
};

public:
	/**
	 * Run Animation
	 * @param timeElapsed Time Elapsed between the last and current frame
	 * @return Current value of animation
	 */
	T Update( float timeElapsed )
	{
		//Animation is running?
		if( canPlayAnimation )
		{
			//Animation has ended
			if( currentTime >= totalTime )
			{
				if( loopAnimation )
					Reset();
				else
					Stop();
			}
			else
			{
				//Update Current Frame
				if( currentTime >= keyFrames[currentKeyFrame].time - (currentKeyFrame >= 1 ? keyFrames[currentKeyFrame-1].time : 0) )
				{
					currentKeyFrame++;
					currentTime = 0;

					if( currentKeyFrame >= (int)keyFrames.size() )
						currentKeyFrame = 0;

					return lastValue;
				}

				//Easing Animation
				if( keyFrames[currentKeyFrame].easing )
					lastValue = keyFrames[currentKeyFrame].easing( currentTime, keyFrames[currentKeyFrame-1].value, keyFrames[currentKeyFrame].value - keyFrames[currentKeyFrame-1].value, keyFrames[currentKeyFrame].time - keyFrames[currentKeyFrame-1].time );
			}

			currentTime += timeElapsed;
		}

		return lastValue;
	}

	/**
	 * Used just to construct animation
	 * @param value Value desired to animation starts
	 */
	ValueAnimation<T>& From( T value )
	{
		if( keyFrames.empty() )
			keyFrames.push_back( KeyFrameAnimation{ 0.0f, value, Easing::Linear.run<T> } );

		return *this;
	}

	/**
	 * Set a new keyframe for animation
	 * @param value Value desired
	 */
	ValueAnimation<T>& To( T value )
	{
		if( !keyFrames.empty() )
			keyFrames.push_back( KeyFrameAnimation{ 500.0f, value, nullptr } );

		return *this;
	}

	/**
	 * Set duration between lastest points of animation
	 * @param time Duration
	 */
	ValueAnimation<T>& During( float time )
	{
		if( !keyFrames.empty() )
		{
			keyFrames[keyFrames.size() - 1].time = time + ( keyFrames.size() - 2 >= 0 ? keyFrames[keyFrames.size() - 2].time : 0.0f );
			totalTime += time;
		}

		return *this;
	}

	/**
	 * Set some easing function to interpolate key frames
	 * @param easing Easing Function
	 */
	ValueAnimation<T>& Ease( EasingAnimation easing = EasingAnimation::Linear )
	{
		if( !keyFrames.empty() )
		{
			std::function<T( float, T, T, float )> easingHandler;

			switch( easing )
			{
			case EasingAnimation::BackIn:
				easingHandler = Easing::BackIn.run<T>;
				break;
			case EasingAnimation::BackInOut:
				easingHandler = Easing::BackInOut.run<T>;
				break;
			case EasingAnimation::BackOut:
				easingHandler = Easing::BackOut.run<T>;
				break;
			case EasingAnimation::BounceIn:
				easingHandler = Easing::BounceIn.run<T>;
				break;
			case EasingAnimation::BounceInOut:
				easingHandler = Easing::BounceInOut.run<T>;
				break;
			case EasingAnimation::BounceOut:
				easingHandler = Easing::BounceOut.run<T>;
				break;
			case EasingAnimation::CircIn:
				easingHandler = Easing::CircIn.run<T>;
				break;
			case EasingAnimation::CircInOut:
				easingHandler = Easing::CircInOut.run<T>;
				break;
			case EasingAnimation::CircOut:
				easingHandler = Easing::CircOut.run<T>;
				break;
			case EasingAnimation::CubicIn:
				easingHandler = Easing::CubicIn.run<T>;
				break;
			case EasingAnimation::CubicInOut:
				easingHandler = Easing::CubicInOut.run<T>;
				break;
			case EasingAnimation::CubicOut:
				easingHandler = Easing::CubicOut.run<T>;
				break;
			case EasingAnimation::ElasticIn:
				easingHandler = Easing::ElasticIn.run<T>;
				break;
			case EasingAnimation::ElasticInOut:
				easingHandler = Easing::ElasticInOut.run<T>;
				break;
			case EasingAnimation::ElasticOut:
				easingHandler = Easing::ElasticOut.run<T>;
				break;
			case EasingAnimation::ExpoIn:
				easingHandler = Easing::ExpoIn.run<T>;
				break;
			case EasingAnimation::ExpoInOut:
				easingHandler = Easing::ExpoInOut.run<T>;
				break;
			case EasingAnimation::ExpoOut:
				easingHandler = Easing::ExpoOut.run<T>;
				break;
			case EasingAnimation::Linear:
				easingHandler = Easing::Linear.run<T>;
				break;
			case EasingAnimation::QuadIn:
				easingHandler = Easing::QuadIn.run<T>;
				break;
			case EasingAnimation::QuadInOut:
				easingHandler = Easing::QuadInOut.run<T>;
				break;
			case EasingAnimation::QuadOut:
				easingHandler = Easing::QuadOut.run<T>;
				break;
			case EasingAnimation::QuartIn:
				easingHandler = Easing::QuartIn.run<T>;
				break;
			case EasingAnimation::QuartInOut:
				easingHandler = Easing::QuartInOut.run<T>;
				break;
			case EasingAnimation::QuartOut:
				easingHandler = Easing::QuartOut.run<T>;
				break;
			case EasingAnimation::QuintIn:
				easingHandler = Easing::QuintIn.run<T>;
				break;
			case EasingAnimation::QuintInOut:
				easingHandler = Easing::QuintInOut.run<T>;
				break;
			case EasingAnimation::QuintOut:
				easingHandler = Easing::QuintOut.run<T>;
				break;
			case EasingAnimation::SineIn:
				easingHandler = Easing::SineIn.run<T>;
				break;
			case EasingAnimation::SineInOut:
				easingHandler = Easing::SineInOut.run<T>;
				break;
			case EasingAnimation::SineOut:
				easingHandler = Easing::SineOut.run<T>;
				break;
			}

			keyFrames[keyFrames.size() - 1].easing = easingHandler;
		}

		return *this;
	}

	//! Set a Key Frame on Animation.
	void SetKeyFrame( float time, T value, EasingAnimation easing = EasingAnimation::Linear )
	{
		if( keyFrames.empty() )
		{
			canPlayAnimation = true;
			SetLoop( true );
		}
		
		keyFrames.push_back( KeyFrameAnimation{ time, value, nullptr } );	
		Ease( easing );

		totalTime = time;
	}

	//! Set loop mode on animation.
	void SetLoop( bool value ) { loopAnimation = value; }

	//! Play animation.
	void Play() { canPlayAnimation = true; }

	//! Stop animation.
	void Stop() { canPlayAnimation = false; hasEnded = true; }

	//! Reset animation for the beginning.
	void Reset()
	{
		if( !keyFrames.empty() )
		{
			currentTime = 0.0f;
			currentKeyFrame = 0;
		}
	}

	//! Get Initial Value.
	const T InitialValue() const { return keyFrames[0].value; }

	//! Check if animation is running.
	const bool IsPlaying() const { return canPlayAnimation; }

	//! Check if animation is looping.
	const bool IsLooping() const { return loopAnimation; }

	//! Check if animation has ended.
	const bool HasEnded() const { return hasEnded; }
private:
	std::vector<KeyFrameAnimation> keyFrames;	//!< List to store animation key frames

	int currentKeyFrame = 0;	//!< Current Frame of animation
	float currentTime = 0.0f;	//!< Current Time of animation
	float totalTime = 0.0f;	//!< Duration of final animation
	T lastValue;	//!< Last Value from easing

	bool loopAnimation = false;	//!< Repeat animation when finish
	bool canPlayAnimation = false;	//!< Run animation flag
	bool hasEnded = false;	//!< Animation has ended
};

}