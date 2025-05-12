//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for node edit

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef C_SDNDENODEEDITWIDGET_HPP
#define C_SDNDENODEEDITWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.hpp"

#include "C_SdNdeHalcWidget.hpp"
#include "C_SdNdeDpEditWidget.hpp"
#include "C_SdNdeDalTabContentWidget.hpp"
#include "C_SdBueComIfDescriptionWidget.hpp"
#include "C_SdNdeNodePropertiesTabContentWidget.hpp"
#include "C_SdNdeCoWidget.hpp"
/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdNdeNodeEditWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeNodeEditWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeNodeEditWidget(const uint32_t ou32_NodeIndex, const int32_t os32_TabIndex,
                                  QWidget * const opc_Parent = NULL);
   ~C_SdNdeNodeEditWidget() override;

   void InitStaticNames(void) const;
   bool WasChanged(void) const;
   void Save(void) const;
   void SetFlag(const uint32_t ou32_Flag) const;
   void OpenDetail(const int32_t os32_MainIndex, const int32_t os32_ListIndex, const int32_t os32_ElementIndex,
                   const int32_t os32_Flag);
   void AddFromTsp(const bool oq_IsNewNode);

   int32_t GetTabIndex(void) const;

   static const int32_t hs32_TAB_INDEX_PROPERTIES;
   static const int32_t hs32_TAB_INDEX_DATA_POOL;
   static const int32_t hs32_TAB_INDEX_COMM;
   static const int32_t hs32_TAB_INDEX_CO_MANAGER;
   static const int32_t hs32_TAB_INDEX_HALC;
   static const int32_t hs32_TAB_INDEX_DATA_LOGGER;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChanged(void);
   void SigErrorChange(void);
   void SigSwitchToDataPool(const uint32_t & oru32_BusIndex, const QString & orc_BusName);
   void SigSwitchToBus(const uint32_t & oru32_BusIndex, const QString & orc_BusName);
   void SigSwitchToBusProperties(const uint32_t & oru32_BusIndex, const QString & orc_BusName);
   void SigSwitchToBusProtocol(const uint32_t ou32_Index, const QString & orc_BusName,
                               const stw::opensyde_core::C_OscCanProtocol::E_Type oe_ProtocolType) const;
   void SigSwitchToBusProtocolMessage(const uint32_t ou32_Index, const QString & orc_BusName,
                                      const stw::opensyde_core::C_OscCanMessageIdentificationIndices & orc_MessageId)
   const;

   void SigSwitchToDeviceNodeInCoManager(const uint32_t ou32_ManagerNodeIndex, const QString & orc_ManagerNodeName,
                                         const uint32_t ou32_DeviceNodeIndex) const;
   void SigNameChanged(const QString & orc_Name, const QString & orc_SubItemName,
                       const bool oq_CombineItemAndSubSubName);
   void SigSave(void);
   void SigSaveAs(void);

protected:
   void showEvent(QShowEvent * const opc_Event) override;
   void hideEvent(QHideEvent * const opc_Event) override;

private:
   //Avoid call
   C_SdNdeNodeEditWidget(const C_SdNdeNodeEditWidget &);
   C_SdNdeNodeEditWidget & operator =(const C_SdNdeNodeEditWidget &) &;

   void m_LoadUserSettings(void) const;
   void m_SaveUserSettings(void) const;
   void m_DataChanged(void);
   void m_OnSwitchToBus(const uint32_t & oru32_BusIndex, const QString & orc_BusName);
   void m_OnSwitchToHalc(void);
   void m_OnSwitchToCommMessages(const uint32_t ou32_DataPoolIndex);
   void m_OnSwitchToCanOpenManager(void);
   void m_CurrentTabChanged(const int32_t os32_Index);
   void m_TabClicked(const int32_t os32_Index) const;
   void m_CreateTabWidgetsAlways(const int32_t os32_Index, const bool oq_AdaptCursor);
   void m_OnDpChanged(void);
   void m_ReloadDataPools(void) const;
   void m_ReloadCanOpenConfig(void) const;
   void m_ReloadCommMessages(void) const;
   void m_ReloadCommDatapools(void) const;
   void m_ReloadHalc(void) const;
   void m_ReloadDataLogger(void) const;
   void m_ReloadApplications(void) const;
   void m_HalcLoadedFromTsp(void) const;
   void m_CreatePropertiesTab(const bool oq_AdaptCursor);
   void m_CreateDpTab(const bool oq_AdaptCursor);
   void m_CreateCommTab(const bool oq_AdaptCursor);
   void m_CreateHalTab(const bool oq_AdaptCursor);
   void m_CreateCoManagerTab(const bool oq_AdaptCursor);
   void m_CreateDataLoggerTab(const bool oq_AdaptCursor);
   static void mh_StartWaitingCursor(void);
   static void mh_EndWaitingCursor(void);
   void m_UpdateTrigger(void);
   void m_PropAndCoTabUpdateTrigger(const uint32_t ou32_NodeIndex);
   void m_HandleVisibleTabs(void);
   void m_FixInitialTabIndex(int32_t & ors32_TabIndex) const;

   Ui::C_SdNdeNodeEditWidget * mpc_Ui;
   const uint32_t mu32_NodeIndex;
   bool mq_DataChanged;
   bool mq_SkipLoadUserSettings;
   C_SdNdeNodePropertiesTabContentWidget * mpc_PropertiesWidget;
   C_SdNdeDpEditWidget * mpc_DataPoolEditWidget;
   C_SdBueComIfDescriptionWidget * mpc_ComIfDescriptionWidget;
   C_SdNdeHalcWidget * mpc_HalWidget;
   C_SdNdeCoWidget * mpc_CoManagerWidget;
   C_SdNdeDalTabContentWidget * mpc_DataLoggerWidget;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
