#pragma once
namespace Graphics
{
	/**
	Represents the time in seconds.
	*/
	typedef float Seconds;

	/**
	TODO: description
	*/
	class Clock
	{
	private:
		Seconds startTime;
		Seconds currentTime, previousTime;
	public:
		static Seconds GetCurrentTime();
	
		Clock();
		/**
		TODO: description
		*/
		Seconds ElapsedTime() const;
		Seconds TotalTime() const;
		/**
		TODO: description
		*/
		void Tick();
	};
}
