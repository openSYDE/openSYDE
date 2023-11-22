//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Bus edit widget (note: main module description should be in .cpp file)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEBUSEDITWIDGET_HPP
#define C_SDBUEBUSEDITWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>

#include "stwtypes.hpp"

#include "C_OscSystemBus.hpp"
#include "C_SdBueComIfDescriptionWidget.hpp"
#include "C_SdBueBusEditPropertiesWidget.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdBueBusEditWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueBusEditWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueBusEditWidget(const uint32_t ou32_BusIndex, const int32_t os32_TabIndex,
                                 QWidget * const opc_Parent = NULL);
   ~C_SdBueBusEditWidget() override;

   void InitStaticNames(void) const;
   bool WasChanged(void) const;
   void Save(void) const;
   void SetFlag(const uint32_t ou32_Flag) const;
   void OpenDetail(const int32_t os32_NodeIndex, const int32_t os32_DataPoolIndex, const int32_t os32_ListIndex,
                   const int32_t os32_ElementIndex, const int32_t os32_Flag) const;
   int32_t GetTabIndex(void) const;

   void ImportMessages(void);

   static const int32_t hs32_TAB_INDEX_PROPERTIES;
   static const int32_t hs32_TAB_INDEX_COMM;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChanged(void);
   void SigErrorChange(void);
   void SigNameChanged(const QString & orc_Name, const QString & orc_SubItemName = "",
                       const bool oq_CombineItemAndSubSubName = false);
   void SigSwitchToCoManager(const uint32_t ou32_NodeIndex, const QString & orc_NodeName,
                             const uint8_t ou8_InterfaceNumber) const;

protected:
   void showEvent(QShowEvent * const opc_Event) override;

private:
   //Avoid call
   C_SdBueBusEditWidget(const C_SdBueBusEditWidget &);
   C_SdBueBusEditWidget & operator =(const C_SdBueBusEditWidget &) &;

   void m_DataChanged(void);
   void m_TabChanged(const int32_t os32_Index);
   void m_CreateTabWidgetsAlways(const int32_t os32_Index, const bool oq_AdaptCursor);
   void m_CreatePropertiesTab(const bool oq_AdaptCursor);
   void m_CreateCommTab(const bool oq_AdaptCursor);
   static void mh_StartWaitingCursor(void);
   static void mh_EndWaitingCursor(void);

   Ui::C_SdBueBusEditWidget * mpc_Ui;
   const uint32_t mu32_BusIndex;
   bool mq_DataChanged;
   stw::opensyde_core::C_OscSystemBus::E_Type me_BusType;
   bool mq_SkipLoadUserSettings;
   C_SdBueBusEditPropertiesWidget * mpc_BusPropertiesWidget;
   C_SdBueComIfDescriptionWidget * mpc_WidgetComIfDescr;
};
}
}

#endif
