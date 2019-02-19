//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Tree widget for searching in the system definition (header)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     14.03.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDSEARCHTREEWIDGET_H
#define C_SDSEARCHTREEWIDGET_H

/* -- Includes ------------------------------------------------------------- */

#include "C_OgeTreeWidgetToolBarSearch.h"
#include "C_OSCNode.h"
#include "C_OSCSystemBus.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdSearchTreeWidget :
   public stw_opensyde_gui_elements::C_OgeTreeWidgetToolBarSearch
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
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;
   virtual void focusOutEvent(QFocusEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_SdSearchTreeWidget(const C_SdSearchTreeWidget &);
   C_SdSearchTreeWidget & operator =(const C_SdSearchTreeWidget &);

   void m_StartSearch(void);
   void m_SearchNodeContent(const stw_opensyde_core::C_OSCNode & orc_Node, const stw_types::uint32 ou32_NodeIndex);
   void m_SearchDataPoolContent(const stw_opensyde_core::C_OSCNodeDataPool & orc_DataPool,
                                const stw_types::uint32 ou32_DataPoolIndex, const QString & orc_NodeName,
                                const stw_types::uint32 ou32_NodeIndex);
   void m_SearchCanProtocolContent(const stw_opensyde_core::C_OSCCanProtocol & orc_CanProtocol,
                                   const QString & orc_NodeName, const stw_types::uint32 ou32_NodeIndex);
   void m_SearchBusContent(const stw_opensyde_core::C_OSCSystemBus & orc_Bus, const stw_types::uint32 ou32_BusIndex);

   bool m_CheckWhereToJump(const QTreeWidgetItem * const opc_Item, stw_types::uint32 & oru32_InterfaceIndex) const;
   void m_ItemClicked(void);

   void m_AddNodeResult(const QString & orc_NodeName, const stw_types::uint32 ou32_NodeIndex,
                        const QString & orc_DeviceType);
   void m_AddBusResult(const QString & orc_BusName, const stw_types::uint32 ou32_BusIndex, const bool oq_Ethernet);
   void m_AddDataPoolResult(const QString & orc_DataPoolName, const stw_types::uint32 ou32_DataPoolIndex,
                            const QString & orc_NodeName, const stw_types::uint32 ou32_NodeIndex);
   void m_AddListResult(const QString & orc_ListName, const stw_types::uint32 ou32_ListIndex,
                        const QString & orc_DataPoolName, const stw_types::uint32 ou32_DataPoolIndex,
                        const QString & orc_NodeName, const stw_types::uint32 ou32_NodeIndex);
   void m_AddMessageResult(const QString & orc_MessageName, const stw_types::uint32 ou32_MessageIndex,
                           const bool oq_IsTx, const stw_types::uint32 ou32_ListIndex, const QString & orc_ProtocolName,
                           const stw_types::uint32 ou32_DataPoolIndex, const QString & orc_NodeName,
                           const stw_types::uint32 ou32_NodeIndex);
   void m_AddDataElementsResult(const QString & orc_DataElementName, const stw_types::uint32 ou32_DataElementIndex,
                                const QString & orc_ListName, const stw_types::uint32 ou32_ListIndex,
                                const QString & orc_DataPoolName, const stw_types::uint32 ou32_DataPoolIndex,
                                const QString & orc_NodeName, const stw_types::uint32 ou32_NodeIndex,
                                const stw_types::uint8 ou8_Type);
   void m_AddApplicationResult(const QString & orc_ApplicationName, const stw_types::uint32 ou32_ApplicationIndex,
                               const QString & orc_NodeName, const stw_types::uint32 ou32_NodeIndex);
   void m_SetupStartingState(void);

   QTreeWidgetItem * mpc_TreeItemRootNodes;
   QTreeWidgetItem * mpc_TreeItemRootBusses;
   QTreeWidgetItem * mpc_TreeItemRootDataPools;
   QTreeWidgetItem * mpc_TreeItemRootLists;
   QTreeWidgetItem * mpc_TreeItemRootMessages;
   QTreeWidgetItem * mpc_TreeItemRootDataElements;
   QTreeWidgetItem * mpc_TreeItemRootApplications;

   stw_types::uint32 mu32_NodesFound;
   stw_types::uint32 mu32_BussesFound;
   stw_types::uint32 mu32_DataPoolsFound;
   stw_types::uint32 mu32_ListsFound;
   stw_types::uint32 mu32_MessagesFound;
   stw_types::uint32 mu32_DataElementsFound;
   stw_types::uint32 mu32_ApplicationsFound;

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

   static const stw_types::sintn mhsn_DATAROLE_TITLE;
   static const stw_types::sintn mhsn_DATAROLE_SUBTITLE;
   static const stw_types::sintn mhsn_DATAROLE_NAME;
   static const stw_types::sintn mhsn_DATAROLE_INDEX_1;
   static const stw_types::sintn mhsn_DATAROLE_INDEX_2;
   static const stw_types::sintn mhsn_DATAROLE_INDEX_3;
   static const stw_types::sintn mhsn_DATAROLE_INDEX_4;
   static const stw_types::sintn mhsn_DATAROLE_INDEX_5;

   static const stw_types::uint8 mhu8_DATAELEMENT_TYPE_PARAMETER;
   static const stw_types::uint8 mhu8_DATAELEMENT_TYPE_VARIABLE;
   static const stw_types::uint8 mhu8_DATAELEMENT_TYPE_SIGNAL;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
