#include "world_draw_queue.h"
#include "model_controller.h"

SGE_WorldDrawQueue* g_sgeWorldDrawQueue = nullptr;

SGE_WorldDrawQueue::SGE_WorldDrawQueue()
{
	if (g_sgeWorldDrawQueue != nullptr) throw std::logic_error("Only one instance of SGE_WorldDrawQueue can be present at a time");
	g_sgeWorldDrawQueue = this;
}

SGE_WorldDrawQueue::~SGE_WorldDrawQueue()
{
	g_sgeWorldDrawQueue = nullptr;
}

void SGE_WorldDrawQueue::init()
{
	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_ADD_LISTENER(eventsManager, Draw);	
}

void SGE_WorldDrawQueue::deinit()
{
	RZUF3_EventsManager* eventsManager = g_scene->getEventsManager();
	_REMOVE_LISTENER(eventsManager, Draw);
}

void SGE_WorldDrawQueue::addToQueue(SGE_ModelController* modelController, int layer, double z)
{
	m_queue[layer][z].push_back(modelController);
}

void SGE_WorldDrawQueue::onDraw(RZUF3_DrawEvent* event)
{
	for (auto& [layer, queue] : m_queue)
	{
		for (auto& [z, queue] : queue)
		{
			for (SGE_ModelController* modelController : queue)
			{
				modelController->draw();
			}
		}
	}
	m_queue.clear();
}
