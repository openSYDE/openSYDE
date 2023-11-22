//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for editing bus properties

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDBUEBUSEDITPROPERTIESWIDGET_HPP
#define C_SDBUEBUSEDITPROPERTIESWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QWidget>

#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdBueBusEditPropertiesWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueBusEditPropertiesWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueBusEditPropertiesWidget(QWidget * const opc_Parent = NULL);
   ~C_SdBueBusEditPropertiesWidget(void) override;

   void InitStaticNames(void) const;
   void SelectName(void) const;
   void SetBusId(const uint32_t ou32_BusIndex);
   void SaveToData(void) const;

private:
   //Avoid call
   C_SdBueBusEditPropertiesWidget(const C_SdBueBusEditPropertiesWidget &);
   C_SdBueBusEditPropertiesWidget & operator =(const C_SdBueBusEditPropertiesWidget &) &;

   Ui::C_SdBueBusEditPropertiesWidget * mpc_Ui;
   uint32_t mu32_BusIndex;

   QString m_GetComboBoxString(const uint32_t ou32_Bitrate) const;
   uint32_t m_GetBitrateFromComboBoxString(const QString & orc_Entry) const;

   void m_CheckBusName(void);
   void m_CheckBusId(void) const;
   void m_TrimmBusName(void) const;
   void m_RegisterChange(void);
   void m_RegisterNameChange(void);
   void m_RegisterIdChange(void);
   void m_LoadFromData(void);
   void m_CanBitrateFixed(void) const;
   void m_HandleUsableForRoutingEnableChanged();
   void m_UpdateUsableForRoutingUi(const bool oq_Enabled);
   bool m_ConfirmDisableRouting();

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChanged(void);
   void SigNameChanged(const QString & orc_Name, const QString & orc_SubItemName,
                       const bool oq_CombineItemAndSubSubName);
};
}
}

#endif
