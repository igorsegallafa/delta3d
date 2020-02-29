#pragma once

namespace Delta3D::IO::SMD
{
struct FrameInfo
{
	int startFrame;
	int endFrame;
};

struct Frame
{
	int startFrame;
	int endFrame;

	int keyFrameStartIndex;
	int keyFrameCount;
};
}