//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for log job properties configuration
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDALLOGJOBPROPERTIESWIDGET_H
#define C_SDNDEDALLOGJOBPROPERTIESWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "C_OscDataLoggerJobProperties.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDalLogJobPropertiesWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDalLogJobPropertiesWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDalLogJobPropertiesWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDalLogJobPropertiesWidget(void) noexcept override;

   void SetNodeDataLoggerJob(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataLoggerJobIndex);
   void Save(void) const;
   void SaveUserSettings(void) const;
   void InitStaticNames(void) const;
   void ReloadAdditionalTrigger(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigLogJobNameModified();

private:
   Ui::C_SdNdeDalLogJobPropertiesWidget * mpc_Ui;
   enum E_LogFileFormat ///< log file types
   {
      eLFF_CSV,
      eLFF_PARQUET,
      eNUM_OF_FORMATS
   };

   enum E_LogTriggerMode ///< local log triggers
   {
      eLLT_ON_CHANGE,
      eLLT_INTERVAL,
      eNUM_OF_LOG_TRIGGERS
   };

   void m_GetSupportedLogFileFormats() const;
   void m_GetSupportedLocalData() const;
   void m_GetSupportedLogJobUseCase() const;
   static QString mh_ConvertTxMethodToName(
      const stw::opensyde_core::C_OscDataLoggerJobProperties::E_UseCase & ore_Type);
   void m_OnLogJobUseCaseChanged(const int32_t os32_NewIndex);
   void m_OnLocalDataChanged(const int32_t os32_NewIndex);
   void m_OnXappSettingsChanged(void);
   void m_CheckDataLoggerName(void);
   void m_DisconnectChangeTriggers(void) const;
   void m_ReconnectChangeTriggers(void) const;
   void m_OnNameEditingFinished();
   void m_TrimLogJobName(void) const;

   uint32_t mu32_NodeIndex;
   uint32_t mu32_DataLoggerJobIndex;
   stw::opensyde_core::C_OscDataLoggerJobProperties mc_Properties;

   //Avoid call
   C_SdNdeDalLogJobPropertiesWidget(const C_SdNdeDalLogJobPropertiesWidget &);
   C_SdNdeDalLogJobPropertiesWidget & operator =(const C_SdNdeDalLogJobPropertiesWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
