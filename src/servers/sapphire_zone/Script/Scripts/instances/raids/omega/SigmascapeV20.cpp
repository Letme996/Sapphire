#include <ScriptObject.h>
#include <Zone/InstanceContent.h>

class SigmascapeV20 : public InstanceContentScript
{
public:
   SigmascapeV20() : InstanceContentScript( 30060 )
   { }

   void onInit( InstanceContentPtr instance ) override
   {
      
      instance->registerEObj( "Unknown0", 2007457, 7244294, 4, { -6.511937f, 0.003125f, -4.958800f }, 1.000000f );
      instance->registerEObj( "Unknown1", 2007457, 7181375, 4, { 12.476670f, -0.015002f, 6.948618f }, 0.991760f );
      instance->registerEObj( "Unknown2", 2007457, 7181377, 4, { 12.570220f, -0.015320f, -9.693607f }, 0.991760f );
      instance->registerEObj( "Unknown3", 2007457, 7329867, 4, { -4.318359f, -0.015320f, 5.722107f }, 0.991760f );
      instance->registerEObj( "Unknown4", 2007457, 7187344, 4, { -1.960858f, 0.001128f, 0.319698f }, 1.000000f );
      instance->registerEObj( "Unknown5", 2007457, 7174526, 4, { -6.872698f, 0.004465f, 1.843464f }, 1.000000f );
      instance->registerEObj( "Unknown6", 2007457, 7174523, 4, { 11.220570f, 0.054783f, -1.564175f }, 1.000000f );
      instance->registerEObj( "Unknown7", 2007457, 7174518, 4, { 8.520967f, 0.009985f, -8.679861f }, 1.000000f );
      instance->registerEObj( "Unknown8", 2007457, 7174519, 4, { 9.028366f, 0.055644f, 3.513119f }, 1.000000f );
      instance->registerEObj( "Unknown9", 2007457, 7180548, 4, { 2.311234f, 0.003611f, 3.656348f }, 1.000000f );
      instance->registerEObj( "Unknown10", 2007457, 7244420, 4, { 2.385169f, 0.003358f, 3.242888f }, 1.000000f );
      instance->registerEObj( "Unknown11", 2007457, 7181938, 4, { 15.676980f, 0.010429f, -11.942520f }, 1.000000f );
      instance->registerEObj( "Unknown12", 2007457, 7181940, 4, { 16.526520f, 0.010429f, 11.441620f }, 1.000000f );
      instance->registerEObj( "Unknown13", 2007457, 7181939, 4, { -16.083740f, 0.010429f, 11.147680f }, 1.000000f );
      instance->registerEObj( "Unknown14", 2007457, 7181937, 4, { -16.250851f, -0.015320f, -11.795230f }, 0.991760f );
      instance->registerEObj( "Entrance", 2000182, 7179724, 5, { -1.443299f, 0.007087f, 6.836350f }, 1.000000f );
      instance->registerEObj( "Exit", 2000139, 0, 4, { 0.000000f, 0.000000f, -11.000000f }, 1.000000f );
      instance->registerEObj( "Unknown16", 2007457, 7171885, 4, { 4.955309f, 0.000000f, -52.142441f }, 1.000000f );
   }

   void onUpdate( InstanceContentPtr instance, uint32_t currTime ) override
   {

   }

   void onEnterTerritory( Entity::Player &player, uint32_t eventId, uint16_t param1, uint16_t param2 ) override
   {

   }

};