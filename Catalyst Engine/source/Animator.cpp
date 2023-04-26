#include <Animator.h>

AnimationClip::AnimationClip()
{
	MaxCountHorizontal = 0;
	MaxCountVertical = 0;

	FrameWidth = 0;
	FrameHeight = 0;

	Loop = 0;

	Sheet = {};

	CurrentFrame = 0;

	FrameTime = 0;
	Counter = 0;

	Frames = std::vector<std::pair<uint32, float>>();
}

AnimationClip::AnimationClip(Texture texture
	, uint32 maxH, uint32 maxV
	, float frameTime
	, bool loop)
{
	MaxCountHorizontal = maxH;
	MaxCountVertical = maxV;

	FrameWidth = 1.0f / (float)maxH;
	FrameHeight = 1.0f / (float)maxV;

	Loop = loop;
	IsRunning = false;

	Sheet = texture;

	CurrentFrame = 0;

	Counter = 0;

	FrameTime = frameTime < (1.0f / 60.0f) ? (1.0f / 60.0f) : frameTime;
	for (uint32 i = 0; i < maxH * maxV; i++)
	{
		Frames.push_back(std::pair<uint32, float>(i, FrameTime));
	}
}

AnimationClip::AnimationClip(Texture texture
	, uint32 maxH, uint32 maxV
	, std::vector<std::pair<uint32, float>> *frames
	, bool loop)
{
	MaxCountHorizontal = maxH;
	MaxCountVertical = maxV;

	FrameWidth = 1.0f / (float)maxH;
	FrameHeight = 1.0f / (float)maxV;

	Loop = loop;
	IsRunning = false;
	Sheet = texture;

	FrameTime = 0;
	CurrentFrame = 0;
	Counter = 0;

	for (uint32 i = 0; i < frames->size(); i++)
	{
		Frames.push_back((*frames)[i]);
	}
}

Animator::Animator(char *name, SpriteRenderer *renderer)
{
	Type = "ANIMATOR";

	Renderer = renderer;
	Name = name;
	LastRunningClip = NULL;
	CurrentClip = NULL;
	StopFlag = false;

	Clips = new std::map<char *, AnimationClip *>();
}

void Animator::Destroy()
{
	std::map<char *, AnimationClip*>::iterator it;

	for (it = (*Clips).begin(); it != (*Clips).end(); it++)
	{
		delete it->second;
	}

	(*Clips).clear();
	delete Clips;
}

void Animator::Update()
{
	if (CurrentClip)
	{
		if (CurrentClip->Loop && CurrentClip->CurrentFrame >= CurrentClip->Frames.size())
		{
			CurrentClip->IsRunning = true;
			CurrentClip->CurrentFrame = 0;
			CurrentClip->Counter;
		}
		else if (!CurrentClip->Loop && CurrentClip->CurrentFrame >= CurrentClip->Frames.size())
		{
			CurrentClip->IsRunning = false;
		}

		if (CurrentClip->CurrentFrame < CurrentClip->Frames.size())
		{
			if (CurrentClip->Counter <= CurrentClip->Frames[CurrentClip->CurrentFrame].second)
			{
				uint32 col = CurrentClip->Frames[CurrentClip->CurrentFrame].first % CurrentClip->MaxCountHorizontal;
				uint32 row = (CurrentClip->MaxCountVertical - 1) - (CurrentClip->Frames[CurrentClip->CurrentFrame].first / CurrentClip->MaxCountHorizontal);

				vec2f bottomLeft = vec2f(float(col) * CurrentClip->FrameWidth, float(row) * CurrentClip->FrameHeight);
				vec2f bottomRight = vec2f((float(col) * CurrentClip->FrameWidth) + CurrentClip->FrameWidth, float(row) * CurrentClip->FrameHeight);
				vec2f topLeft = vec2f(float(col) * CurrentClip->FrameWidth, (float(row) * CurrentClip->FrameHeight) + CurrentClip->FrameHeight);
				vec2f topRight = vec2f((float(col) * CurrentClip->FrameWidth) + CurrentClip->FrameWidth, (float(row) * CurrentClip->FrameHeight) + CurrentClip->FrameHeight);

				Vertex *vertices = Renderer->GetSprite().GetBuffers().OpenVertexBuffer();
			
				vertices[0].Texcoords = bottomLeft;
				vertices[1].Texcoords = bottomRight;
				vertices[2].Texcoords = topRight;
				vertices[3].Texcoords = bottomLeft;
				vertices[4].Texcoords = topRight;
				vertices[5].Texcoords = topLeft;

				Renderer->GetSprite().GetBuffers().CloseVertexBuffer();

				CurrentClip->Counter += 1.0f / 60.0f;
			}
			else
			{
				CurrentClip->CurrentFrame++;
				CurrentClip->Counter = 0;
			}
		}
	}

	if (StopFlag)
	{
		if (IsFinished())
		{
			StopClip();
			StopFlag = false;
		}
	}
}

void Animator::SetStartClip(char *name)
{
	AnimationClip *clip = GetClip(name);

	if (clip)
	{
		CurrentClip = clip;
		Renderer->SetSkin((*Clips)[name]->Sheet);

		uint32 col = CurrentClip->Frames[CurrentClip->CurrentFrame].first % CurrentClip->MaxCountHorizontal;
		uint32 row = (CurrentClip->MaxCountVertical - 1) - (CurrentClip->Frames[CurrentClip->CurrentFrame].first / CurrentClip->MaxCountHorizontal);

		vec2f bottomLeft = vec2f(float(col) * CurrentClip->FrameWidth, float(row) * CurrentClip->FrameHeight);
		vec2f bottomRight = vec2f((float(col) * CurrentClip->FrameWidth) + CurrentClip->FrameWidth, float(row) * CurrentClip->FrameHeight);
		vec2f topLeft = vec2f(float(col) * CurrentClip->FrameWidth, (float(row) * CurrentClip->FrameHeight) + CurrentClip->FrameHeight);
		vec2f topRight = vec2f((float(col) * CurrentClip->FrameWidth) + CurrentClip->FrameWidth, (float(row) * CurrentClip->FrameHeight) + CurrentClip->FrameHeight);

		Vertex *vertices = Renderer->GetSprite().GetBuffers().OpenVertexBuffer();
	
		vertices[0].Texcoords = bottomLeft;
		vertices[1].Texcoords = bottomRight;
		vertices[2].Texcoords = topRight;
		vertices[3].Texcoords = bottomLeft;
		vertices[4].Texcoords = topRight;
		vertices[5].Texcoords = topLeft;

		Renderer->GetSprite().GetBuffers().CloseVertexBuffer();

		LastRunningClip = NULL;
	}
}

void Animator::SwitchClip(char *name)
{
	AnimationClip *clip = GetClip(name);

	if (clip)
	{
		if (clip != CurrentClip)
		{
			StopFlag = false;

			if (CurrentClip)
			{
				StopClip();
			}

			LastRunningClip = CurrentClip;
			CurrentClip = clip;
			StopClip();
			ResumeClip();
			Renderer->SetSkin(clip->Sheet);

			uint32 col = CurrentClip->Frames[CurrentClip->CurrentFrame].first % CurrentClip->MaxCountHorizontal;
			uint32 row = (CurrentClip->MaxCountVertical - 1) - (CurrentClip->Frames[CurrentClip->CurrentFrame].first / CurrentClip->MaxCountHorizontal);

			vec2f bottomLeft = vec2f(float(col) * CurrentClip->FrameWidth, float(row) * CurrentClip->FrameHeight);
			vec2f bottomRight = vec2f((float(col) * CurrentClip->FrameWidth) + CurrentClip->FrameWidth, float(row) * CurrentClip->FrameHeight);
			vec2f topLeft = vec2f(float(col) * CurrentClip->FrameWidth, (float(row) * CurrentClip->FrameHeight) + CurrentClip->FrameHeight);
			vec2f topRight = vec2f((float(col) * CurrentClip->FrameWidth) + CurrentClip->FrameWidth, (float(row) * CurrentClip->FrameHeight) + CurrentClip->FrameHeight);

			Vertex *vertices = Renderer->GetSprite().GetBuffers().OpenVertexBuffer();
			
			vertices[0].Texcoords = bottomLeft;
			vertices[1].Texcoords = bottomRight;
			vertices[2].Texcoords = topRight;
			vertices[3].Texcoords = bottomLeft;
			vertices[4].Texcoords = topRight;
			vertices[5].Texcoords = topLeft;

			Renderer->GetSprite().GetBuffers().CloseVertexBuffer();			
		}
	}
}

void Animator::AddClip(char *name, AnimationClip *clip)
{
	if ((*Clips).find(name) == (*Clips).end())
	{
		AnimationClip *c = new AnimationClip();
		c->Counter = clip->Counter;
		c->CurrentFrame = clip->CurrentFrame;
		c->MaxCountHorizontal = clip->MaxCountHorizontal;
		c->MaxCountVertical = clip->MaxCountVertical;
		c->FrameHeight = clip->FrameHeight;
		c->FrameWidth = clip->FrameWidth;
		c->FrameTime = clip->FrameTime;
		c->Loop = clip->Loop;
		c->Sheet = clip->Sheet;
		c->Name = name;

		for (uint32 i = 0; i < clip->Frames.size(); i++)
		{
			c->Frames.push_back(clip->Frames[i]);
		}

		(*Clips)[name] = c;

		if (!CurrentClip)
		{
			CurrentClip = (*Clips)[name];
			Renderer->SetSkin((*Clips)[name]->Sheet);

			uint32 col = CurrentClip->Frames[CurrentClip->CurrentFrame].first % CurrentClip->MaxCountHorizontal;
			uint32 row = (CurrentClip->MaxCountVertical - 1) - (CurrentClip->Frames[CurrentClip->CurrentFrame].first / CurrentClip->MaxCountHorizontal);

			vec2f bottomLeft = vec2f(float(col) * CurrentClip->FrameWidth, float(row) * CurrentClip->FrameHeight);
			vec2f bottomRight = vec2f((float(col) * CurrentClip->FrameWidth) + CurrentClip->FrameWidth, float(row) * CurrentClip->FrameHeight);
			vec2f topLeft = vec2f(float(col) * CurrentClip->FrameWidth, (float(row) * CurrentClip->FrameHeight) + CurrentClip->FrameHeight);
			vec2f topRight = vec2f((float(col) * CurrentClip->FrameWidth) + CurrentClip->FrameWidth, (float(row) * CurrentClip->FrameHeight) + CurrentClip->FrameHeight);

			Vertex *vertices = Renderer->GetSprite().GetBuffers().OpenVertexBuffer();
			
			vertices[0].Texcoords = bottomLeft;
			vertices[1].Texcoords = bottomRight;
			vertices[2].Texcoords = topRight;

			vertices[3].Texcoords = bottomLeft;
			vertices[4].Texcoords = topRight;
			vertices[5].Texcoords = topLeft;

			Renderer->GetSprite().GetBuffers().CloseVertexBuffer();
		}
	}
}

AnimationClip* Animator::GetClip(char *name)
{
	if (Clips->find(name) != Clips->end())
	{
		return (*Clips)[name];
	}

	return NULL;
}

void Animator::PauseClip()
{
	CurrentClip->IsRunning = false;
	LastRunningClip = CurrentClip;
	CurrentClip = NULL;
}

void Animator::ResumeClip()
{
	CurrentClip = LastRunningClip;
	LastRunningClip = NULL;
	CurrentClip->IsRunning = true;
	StopFlag = false;
}

void Animator::StopClip()
{
	CurrentClip->Counter = 0;
	CurrentClip->CurrentFrame = 0;
	CurrentClip->IsRunning = false;
	LastRunningClip = CurrentClip;
	CurrentClip = NULL;
}

bool Animator::IsFinished()
{
	if (CurrentClip)
	{
		if (CurrentClip->CurrentFrame >= CurrentClip->Frames.size())
		{
			return true;
		}
	}

	return false;
}

void Animator::StopWhenDone()
{
	StopFlag = true;
}

char* Animator::GetCurrentClipName()
{
	if (CurrentClip)
	{
		return CurrentClip->Name;
	}

	return "";
}

char* Animator::GetLastClipName()
{
	if (LastRunningClip)
	{
		return LastRunningClip->Name;
	}

	return "";
}

bool Animator::IsRunning(char *name)
{
	AnimationClip *clip = GetClip(name);
	
	if (clip)
	{
		if (clip->IsRunning)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}