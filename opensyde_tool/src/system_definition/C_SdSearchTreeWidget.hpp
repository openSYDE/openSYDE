//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Tree widget for searching in the system definition (header)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDSEARCHTREEWIDGET_H
#define C_SDSEARCHTREEWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include "C_OgeTreeWidgetToolBarSearch.hpp"
#include "C_OscNode.hpp"
#include "C_OscSystemBus.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdSearchTreeWidget :
   public stw::opensyde_gui_elements::C_OgeTreeWidgetToolBarSearch
{
   Q_OBJECT

public:
   C_SdSearchTreeWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdSearchTreeWidget();

   virtual bool Search(const QString & orc_SearchString);
   virtual void ClearResult(void);
   virtual void SetSearchResultFocus(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   void SigHide(void);
   void SigFocusOut(void);
   //lint -restore

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   void focusOutEvent(QFocusEvent * const opc_Event) override;

private:
   //Avoid call
   C_SdSearchTreeWidget(const C_SdSearchTreeWidget &);
   C_SdSearchTreeWidget & operator =(const C_SdSearchTreeWidget &) &;

   void m_StartSearch(void);
   void m_SearchNodeContent(const stw::opensyde_core::C_OscNode & orc_Node, const uint32_t ou32_NodeIndex);
   void m_SearchDataPoolContent(const stw::opensyde_core::C_OscNodeDataPool & orc_DataPool,
                                const uint32_t ou32_DataPoolIndex, const QString & orc_NodeName,
                                const uint32_t ou32_NodeIndex);
   void m_SearchCanProtocolContent(const stw::opensyde_core::C_OscCanProtocol & orc_CanProtocol,
                                   const QString & orc_NodeName, const uint32_t ou32_NodeIndex);
   void m_SearchHalcConfigurationContent(const stw::opensyde_core::C_OscHalcConfig & orc_HalcConfig,
                                         const QString & orc_NodeName, const uint32_t ou32_NodeIndex);
   void m_SearchBusContent(const stw::opensyde_core::C_OscSystemBus & orc_Bus, const uint32_t ou32_BusIndex);

   bool m_CheckWhereToJumpCan(const QTreeWidgetItem * const opc_Item, uint32_t & oru32_InterfaceIndex) const;
   void m_ItemClicked(void);

   void m_AddNodeResult(const QString & orc_NodeName, const uint32_t ou32_NodeIndex, const QString & orc_DeviceType);
   void m_AddBusResult(const QString & orc_BusName, const uint32_t ou32_BusIndex, const bool oq_Ethernet);
   void m_AddDataPoolResult(const QString & orc_DataPoolName, const uint32_t ou32_DataPoolIndex,
                            const QString & orc_NodeName, const uint32_t ou32_NodeIndex);
   void m_AddListResult(const QString & orc_ListName, const uint32_t ou32_ListIndex, const QString & orc_DataPoolName,
                        const uint32_t ou32_DataPoolIndex, const QString & orc_NodeName, const uint32_t ou32_NodeIndex);
   void m_AddMessageResult(const QString & orc_MessageName, const uint32_t ou32_MessageIndex, const bool oq_IsTx,
                           const uint32_t ou32_ListIndex, const QString & orc_ProtocolName,
                           const uint32_t ou32_DataPoolIndex, const QString & orc_NodeName,
                           const uint32_t ou32_NodeIndex);
   void m_AddDataElementsResult(const QString & orc_DataElementName, const uint32_t ou32_DataElementIndex,
                                const QString & orc_ListName, const uint32_t ou32_ListIndex,
                                const QString & orc_DataPoolName, const uint32_t ou32_DataPoolIndex,
                                const QString & orc_NodeName, const uint32_t ou32_NodeIndex, const uint8_t ou8_Type);
   void m_AddApplicationResult(const QString & orc_ApplicationName, const uint32_t ou32_ApplicationIndex,
                               const QString & orc_NodeName, const uint32_t ou32_NodeIndex);
   void m_AddHalcChannelResult(const QString & orc_HalcDomainName, const uint32_t ou32_HalcDomainIndex,
                               const QString & orc_HalcChannelName, const uint32_t ou32_HalcChannelIndex,
                               const bool oq_UseChannelIndex, const QString & orc_NodeName,
                               const uint32_t ou32_NodeIndex,
                               const stw::opensyde_core::C_OscHalcDefDomain::E_Category oe_Category);
   void m_SetupStartingState(void);

   QTreeWidgetItem * mpc_TreeItemRootNodes;
   QTreeWidgetItem * mpc_TreeItemRootBusses;
   QTreeWidgetItem * mpc_TreeItemRootDataPools;
   QTreeWidgetItem * mpc_TreeItemRootLists;
   QTreeWidgetItem * mpc_TreeItemRootMessages;
   QTreeWidgetItem * mpc_TreeItemRootDataElements;
   QTreeWidgetItem * mpc_TreeItemRootApplications;
   QTreeWidgetItem * mpc_TreeItemRootHalcChannels;

   uint32_t mu32_NodesFound;
   uint32_t mu32_BussesFound;
   uint32_t mu32_DataPoolsFound;
   uint32_t mu32_ListsFound;
   uint32_t mu32_MessagesFound;
   uint32_t mu32_DataElementsFound;
   uint32_t mu32_ApplicationsFound;
   uint32_t mu32_HalcChannelsFound;

   QIcon mc_IconNode;
   QIcon mc_IconBusCan;
   QIcon mc_IconBusEthernet;
   QIcon mc_IconMessage;
   QIcon mc_IconSignal;
   QIcon mc_IconVariable;
   QIcon mc_IconParameter;
   QIcon mc_IconList;
   QIcon mc_IconDataPool;
   QIcon mc_IconApplication;
   QIcon mc_IconHalcChannel;
   QIcon mc_IconHalcInputChannel;
   QIcon mc_IconHalcOutputChannel;

   static const int32_t mhs32_DATAROLE_TITLE;
   static const int32_t mhs32_DATAROLE_SUBTITLE;
   static const int32_t mhs32_DATAROLE_NAME;
   static const int32_t mhs32_DATAROLE_INDEX_1;
   static const int32_t mhs32_DATAROLE_INDEX_2;
   static const int32_t mhs32_DATAROLE_INDEX_3;
   static const int32_t mhs32_DATAROLE_INDEX_4;
   static const int32_t mhs32_DATAROLE_INDEX_5;

   static const uint8_t mhu8_DATAELEMENT_TYPE_PARAMETER;
   static const uint8_t mhu8_DATAELEMENT_TYPE_VARIABLE;
   static const uint8_t mhu8_DATAELEMENT_TYPE_SIGNAL;
   static const uint8_t mhu8_DATAELEMENT_TYPE_HALC;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
