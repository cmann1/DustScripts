#include 'common.cpp';

class EmitterBurstManager
{
	
	protected scene@ g;
	
	protected int bursts_size = 16;
	protected int bursts_count = 0;
	protected array<EmitterBurst> bursts(bursts_size);
	
	EmitterBurstManager(scene@ g)
	{
		@this.g = g;
	}
	
	/// Make sure to call during `checkpoint_load`
	void reset()
	{
		bursts_count = 0;
	}
	
	/// `frame_inc` - Either 1 or `scene::time_warp`.
	void update(const float frame_inc)
	{
		for(int i = bursts_count - 1; i >= 0; i--)
		{
			EmitterBurst@ burst = @bursts[i];
			burst.time -= frame_inc;
			if(burst.time <= 0)
			{
				g.remove_entity(burst.e);
				bursts[i] = bursts[--bursts_count];
			}
		}
	}
	
	/// Add a short lived particle burst based on the given settings.
	void add(const EmitterBurstSettings@ settings, const float x, const float y, const float rotation=0)
	{
		for(int i = 0; i < settings.count; i++)
		{
			entity@ e = create_emitter(settings.emitter_id,
				x, y,
				int(settings.size_x), int(settings.size_y),
				settings.layer, settings.sub_layer, int(rotation)
			);
			g.add_entity(e, false);
			
			if(bursts_count >= bursts_size)
			{
				bursts.resize(bursts_size *= 2);
			}
			
			EmitterBurst@ burst = @bursts[bursts_count++];
			burst.init(e, settings.time);
		}
	}
	
}

class EmitterBurst
{
	
	entity@ e;
	float time;

	EmitterBurst() { }

	void init(entity@ e, const float time=15)
	{
		@this.e = e;
		this.time = time;
	}
	
}

class EmitterBurstSettings
{
	
	int emitter_id;
	int layer, sub_layer;
	float size_x, size_y;
	float time;
	int count;
	
	EmitterBurstSettings() {}
	
	EmitterBurstSettings(
		const int emitter_id,
		const int layer, const int sub_layer,
		const float size_x, const float size_y,
		const float time=15, const int count=1)
	{
		this.emitter_id = emitter_id;
		this.layer = layer;
		this.sub_layer = sub_layer;
		this.size_x = size_x;
		this.size_y = size_y;
		this.time = time;
		this.count = count;
	}
	
}
