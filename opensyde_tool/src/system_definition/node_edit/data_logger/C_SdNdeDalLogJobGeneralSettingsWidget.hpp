//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Creating General X-App settings
   \copyright   Copyright 2025 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDALLOGJOBGENERALSETTINGSWIDGET_H
#define C_SDNDEDALLOGJOBGENERALSETTINGSWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "C_OscXappProperties.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDalLogJobGeneralSettingsWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDalLogJobGeneralSettingsWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDalLogJobGeneralSettingsWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDalLogJobGeneralSettingsWidget(void) override;
   void SetNode(const uint32_t ou32_NodeIndex);
   void Save(void);
   void SaveUserSettings(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigReloadDataLoggerDataElements(void);

private:
   Ui::C_SdNdeDalLogJobGeneralSettingsWidget * mpc_Ui;
   uint32_t mu32_NodeIndex;
   stw::opensyde_core::C_OscXappProperties mc_Properties;

   void m_GetSupportedClientInterfaces() const;
   void m_OnClientInterfaceChanged(const int32_t os32_NewIndex);
   void m_OnXappSettingsChanged(void);
   void m_DisconnectChangeTriggers(void) const;
   void m_ReconnectChangeTriggers(void) const;
   //Avoid call
   C_SdNdeDalLogJobGeneralSettingsWidget(const C_SdNdeDalLogJobGeneralSettingsWidget &);
   C_SdNdeDalLogJobGeneralSettingsWidget & operator =(const C_SdNdeDalLogJobGeneralSettingsWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
