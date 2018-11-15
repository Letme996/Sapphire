#include "HousingMgr.h"
#include <Logging/Logger.h>
#include <Database/DatabaseDef.h>
#include <Exd/ExdDataGenerated.h>
#include <Network/PacketContainer.h>
#include <Network/PacketDef/Zone/ServerZoneDef.h>
#include <Network/PacketWrappers/ActorControlPacket142.h>
#include <Network/PacketWrappers/ActorControlPacket143.h>
#include <Network/CommonActorControl.h>

#include <unordered_map>

#include "Actor/Player.h"

#include "Zone.h"
#include "HousingZone.h"
#include "HousingMgr.h"
#include "Land.h"
#include "Framework.h"
#include "ServerZone.h"

using namespace Core::Common;
using namespace Core::Network;
using namespace Core::Network::Packets;
using namespace Core::Network::Packets::Server;

extern Core::Framework g_fw;

Core::HousingMgr::HousingMgr() :
  m_lastLandId( 0 )
{

}

Core::HousingMgr::~HousingMgr()
{

}

bool Core::HousingMgr::init()
{

  return true;
}

uint16_t Core::HousingMgr::getNexLandId()
{
  return ++m_lastLandId;
}

void Core::HousingMgr::insertHousingZone( Core::Data::HousingZonePtr hZone )
{
  uint16_t id = getNexLandId();
  m_housingZonePtrMap[id] = hZone;
}

Core::Data::HousingZonePtr Core::HousingMgr::getHousingZone( uint16_t id )
{
  auto it = m_housingZonePtrMap.find( id );
  if( it == m_housingZonePtrMap.end() )
    return nullptr;

  return it->second;
}

Core::Data::HousingZonePtr Core::HousingMgr::getHousingZoneByLandSetId( uint32_t id )
{
  for( const auto& hZoneIt : m_housingZonePtrMap )
  {
    auto pHousingZone = hZoneIt.second;
    for( uint8_t landId = 0; landId < 60; landId++ )
    {
      if( pHousingZone->getLandSetId() == id )
      {
        return pHousingZone;
      }
    }
  }
  return nullptr;
}

Core::LandPtr Core::HousingMgr::getLandByOwnerId( uint32_t id )
{
  for( const auto& hZoneIt : m_housingZonePtrMap )
  {
    auto pHousingZone = hZoneIt.second;
    for( uint8_t landId = 0; landId < 60; landId++ )
    {
      if( pHousingZone->getLand( landId )->getPlayerOwner() == id )
      {
        return pHousingZone->getLand( landId );
      }
    }
  }
  return nullptr;
}

void Core::HousingMgr::sendLandSignOwned( Entity::Player& player, uint8_t ward, uint8_t plot )
{
  player.setActiveLand( plot, ward );

  auto zone = player.getCurrentZone();

  auto hZone = std::dynamic_pointer_cast< HousingZone >( zone );

  if( !hZone )
    return;

  auto land = hZone->getLand( plot );
  if( !land )
  {
    land = getLandByOwnerId( player.getId() );
  }

  auto landInfoSignPacket = makeZonePacket< Server::FFXIVIpcLandInfoSign >( player.getId() );
  uint32_t playerId = land->getPlayerOwner();
  std::string playerName = g_fw.get< Core::ServerZone >()->getPlayerNameFromDb( playerId );
  //memcpy( &landInfoSignPacket->data().estateGreeting, "Hello World", 11 );
  //memcpy( &landInfoSignPacket->data().estateName, land->getLandName().c_str(), land->getLandName().size() );
  landInfoSignPacket->data().houseSize = land->getSize();
  landInfoSignPacket->data().houseType = static_cast< uint8_t >( land->getLandType() );
  landInfoSignPacket->data().landId = land->getLandId();
  landInfoSignPacket->data().ownerId = player.getContentId(); // should be real owner contentId, not player.contentId()
  memcpy( &landInfoSignPacket->data().ownerName, playerName.c_str(), playerName.size() );
  landInfoSignPacket->data().wardNum = land->getWardNum();
  landInfoSignPacket->data().worldId = 67;
  landInfoSignPacket->data().zoneId = land->getZoneId();
  player.queuePacket( landInfoSignPacket );
}

void Core::HousingMgr::sendLandSignFree( Entity::Player& player, uint8_t ward, uint8_t plot )
{
  player.setActiveLand( plot, ward );

  auto zone = player.getCurrentZone();
  auto hZone = std::dynamic_pointer_cast< HousingZone >( zone );

  if( !hZone )
    return;

  auto land = hZone->getLand( plot );
  auto plotPricePacket = makeZonePacket< Server::FFXIVIpcLandPriceUpdate >( player.getId() );
  plotPricePacket->data().price = land->getCurrentPrice();
  plotPricePacket->data().timeLeft = land->getDevaluationTime();
  player.queuePacket( plotPricePacket );
}

Core::LandPurchaseResult Core::HousingMgr::purchseLand( Entity::Player& player, uint8_t plot, uint8_t state )
{
  auto pHousing = std::dynamic_pointer_cast< HousingZone >( player.getCurrentZone() );

  auto plotPrice = pHousing->getLand( plot )->getCurrentPrice();
  auto gilAvailable = player.getCurrency( CurrencyType::Gil );
  auto pLand = pHousing->getLand( plot );

  if( !pLand )
    return LandPurchaseResult::ERR_INTERNAL;

  if( pLand->getState() != HouseState::forSale )
    return LandPurchaseResult::ERR_NOT_AVAILABLE;
  
  if( gilAvailable < plotPrice )
    return LandPurchaseResult::ERR_NOT_ENOUGH_GIL;
  
  
  switch( static_cast< LandPurchaseMode >( state ) )
  {
    case LandPurchaseMode::FC:
      player.sendDebug( "Free company house purchase aren't supported at this time." );
      return LandPurchaseResult::ERR_INTERNAL;
  
    case LandPurchaseMode::PRIVATE:
    {

      auto pOldLand = getLandByOwnerId( player.getId() );

      if( pOldLand )
        return LandPurchaseResult::ERR_NO_MORE_LANDS_FOR_CHAR;
      player.removeCurrency( CurrencyType::Gil, plotPrice );
      pLand->setPlayerOwner( player.getId() );
      pLand->setState( HouseState::sold );
      pLand->setLandType( Common::LandType::Private );
      player.setLandPermissions( LandPermissionSlot::Private, 0x00, plot,
                                 pHousing->getWardNum(), pHousing->getTerritoryTypeId() );
      player.sendLandPermissions();
      //pLand->setLandName( "Private Estate" + std::to_string( pHousing->getWardNum() ) + "-" + std::to_string( plot )   );
      pLand->updateLandDb();
      pHousing->sendLandUpdate( plot );
      return LandPurchaseResult::SUCCESS;
    }
  
    default:
      return LandPurchaseResult::ERR_INTERNAL;
  }
  
}

