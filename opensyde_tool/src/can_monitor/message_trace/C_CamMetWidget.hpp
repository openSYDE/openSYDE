//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing entire CAN trace and its buttons (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMETWIDGET_H
#define C_CAMMETWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QTimer>

#include "stwtypes.hpp"

#include "C_OgeWiOnlyBackground.hpp"

#include "C_SyvComMessageMonitor.hpp"
#include "C_CamProFilterData.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CamMetWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMetWidget :
   public stw::opensyde_gui_elements::C_OgeWiOnlyBackground
{
   Q_OBJECT

public:
   explicit C_CamMetWidget(QWidget * const opc_Parent = NULL);
   ~C_CamMetWidget(void) override;

   void LoadUserSettings(void);
   void SaveUserSettings(void) const;

   void StopLogging(void);

   void SearchBarFocus(void) const;
   void SearchNext(void) const;
   void SearchPrev(void) const;

   void ClearData(void);

   // Filter configuration
   void AddFilterItems(const QList<stw::opensyde_gui_logic::C_CamProFilterItemData> & orc_FilterItems);
   void RemoveFilterItems(const QList<stw::opensyde_gui_logic::C_CamProFilterItemData> & orc_FilterItems);
   void RemoveAllFilters(void);
   void SetActiveFilters(const uint32_t ou32_ActiveFilters) const;

   // Database handling
   void AddDatabaseOsySysDef(const QString & orc_PathSystemDefinition, const QString & orc_OrgPath,
                             const int32_t os32_BusIndex);
   void AddDatabaseDbc(const QString & orc_PathDbc);
   void RemoveDatabase(const QString & orc_PathDatabase, const QString & orc_OrgPath);
   void ActivateDatabase(const QString & orc_PathDatabase, const QString & orc_OrgPath, const bool oq_Active);
   void SetDatabaseOsySysDefBus(const QString & orc_PathSystemDefinition, const QString & orc_OrgPath,
                                const uint32_t ou32_BusIndex);

   // Logging configuration
   void AddLogFileAsc(const QString & orc_FilePath);
   void AddLogFileBlf(const QString & orc_FilePath);
   void RemoveAllLogFiles(void) const;

   stw::opensyde_gui_logic::C_SyvComMessageMonitor * GetMessageMonitor(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

   void SetCanBitrate(const int32_t os32_Value);

Q_SIGNALS:
   void SigStartLogging(void);
   void SigPauseLogging(void);
   void SigContinueLogging(void);
   void SigStopLogging(void);

   // Values of os32_Result:
   // C_NO_ERR    required data from file successfully stored in orc_Definition and DBC file added to interpretation
   // C_RANGE     orc_File is empty string or has a wrong extension
   // C_CONFIG    orc_File does not point to a valid file
   // C_RD_WR     error while reading file
   // C_WARN      unknown parameter found -> default value set and warning reported
   //             it is a warning only, required data from file successfully stored in orc_Definition and
   //             DBC file added to interpretation
   void SigDatabaseLoadResultDbc(const int32_t os32_Result);
   // Values of os32_Result:
   // C_NO_ERR    data read
   // C_RANGE     specified system definition file does not exist or has a wrong extension
   // C_NOACT     specified file is present but structure is invalid (e.g. invalid XML file)
   // C_CONFIG    system definition file content is invalid or incomplete
   //             device definition file could not be loaded
   // C_OVERFLOW  node in system definition references a device not part of the device definitions
   // C_COM       no CAN bus in system definition
   // C_WARN      specified bus index was not found or is no CAN bus
   void SigDatabaseLoadResultOsySysDef(const int32_t os32_Result,
                                       const std::vector<stw::opensyde_core::C_OscSystemBus> & orc_Busses);
   // Values of os32_Result:
   // C_NO_ERR    Bus index for this system definition adapted
   // C_NOACT     No system definition found with this path
   // C_WARN      specified bus index was not found or is no CAN bus
   void SigDatabaseSetOsySysDefBusResult(const QString & orc_PathSystemDefinition, const int32_t os32_Result);
   // Values of os32_Result:
   // C_NO_ERR    File added successfully
   // C_RD_WR     Error on creating file, folders or deleting old file
   void SigLogFileAddResult(const int32_t os32_Result);
   //lint -restore

private:
   //Avoid call
   C_CamMetWidget(const C_CamMetWidget &);
   C_CamMetWidget & operator =(const C_CamMetWidget &) &;

   void m_StartLogging(void);
   void m_StatusBarTimer(void);
   void m_DatabaseTimer(void);

   Ui::C_CamMetWidget * mpc_Ui;
   QTimer mc_StatusBarTimer;
   QTimer mc_DatabaseTimer;
   bool mq_OsySysDef;
   int32_t ms32_CanBitrate;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
