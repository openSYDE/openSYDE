//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing all possible openSYDE CAN Monitor settings
 (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMOSWIDGET_HPP
#define C_CAMMOSWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "C_OgeWiOnlyBackground.hpp"
#include "C_CamProFilterData.hpp"
#include "C_CamMosSectionPopup.hpp"
#include "C_CamOgePubSettingsAdd.hpp"
#include "C_UsHandler.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CamMosWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMosWidget :
   public stw::opensyde_gui_elements::C_OgeWiOnlyBackground
{
   Q_OBJECT

public:
   explicit C_CamMosWidget(QWidget * const opc_Parent = NULL);
   ~C_CamMosWidget(void) override;

   void LoadUserSettings(void);
   void SaveUserSettings(void) const;

   void OnLoadFinishedDbc(const int32_t os32_Result) const;
   void OnLoadFinishedOsySysDef(const int32_t os32_Result,
                                const std::vector<stw::opensyde_core::C_OscSystemBus> & orc_Busses) const;
   void OnSigOsySysDefBusResult(const QString & orc_PathSystemDefinition, const int32_t os32_Result) const;
   void OnCommunicationStarted(const bool oq_Online) const;
   void OnSigLogFileAddResult(const int32_t os32_Result) const;
   void OnSigSavedAsNew(void) const;
   void OnDatabaseDropped(const QString & orc_Path) const;
   void ClearConfiguration(void) const;

   void ExpandSettings(const bool oq_Expand) const;
   void AddFilterData(const QList<int32_t> oc_CanMsgId, const QList<uint8_t> oc_CanMsgXtd);
   void CanFilterMsgDropped(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigExpandSettings(const bool oq_Expand); // true: expand settings, false: minimize settings
   void SigFilterNumberChanged(const uint32_t ou32_FilterNumber);
   void SigAddFilterItems(const QList<stw::opensyde_gui_logic::C_CamProFilterItemData> & orc_FilterItems);
   void SigRemoveFilterItems(const QList<stw::opensyde_gui_logic::C_CamProFilterItemData> & orc_FilterItems);
   void SigRemoveAllFilters(void);
   void SigAddDatabaseDbc(const QString & orc_Path, const QString & orc_OrgPath);
   void SigAddDatabaseOsySysDef(const QString & orc_Path, const QString & orc_OrgPath, const uint32_t ou32_BusIndex);
   void SigRemoveDatabase(const QString & orc_Path, const QString & orc_OrgPath, const bool oq_IsUpdate);
   void SigActivateDatabase(const QString & orc_Path, const QString & orc_OrgPath, const bool oq_Active);
   void SigSetDatabaseOsySysDefBus(const QString & orc_PathSystemDefinition, const QString & orc_OrgPath,
                                   const uint32_t ou32_BusIndex);
   void SigAddLogFileAsc(const QString & orc_FilePath);
   void SigAddLogFileBlf(const QString & orc_FilePath);
   void SigRemoveAllLogFiles(void);
   void SigCanDllConfigured(void);
   void SigNotifyMissingDataBase(const QString & orc_Path);
   void SigEmitAddFilterToChildWidget(const QList<int32_t> oc_CanMsgId, const QList<uint8_t> oc_CanMsgXtd);
   void SigSendCanMsgDroppedToParentWidget(void);

protected:
   void moveEvent(QMoveEvent * const opc_Event) override;

private:
   Ui::C_CamMosWidget * mpc_Ui;

   stw::opensyde_gui::C_CamMosSectionPopup * mpc_PopupDatabase;
   stw::opensyde_gui::C_CamMosSectionPopup * mpc_PopupDllConfig;
   stw::opensyde_gui::C_CamMosSectionPopup * mpc_PopupFilter;
   stw::opensyde_gui::C_CamMosSectionPopup * mpc_PopupLogging;

   void m_OnExpandSettings(const bool oq_Expand);
   void m_HidePopupDatabase(void) const;
   void m_HidePopupDllConfig(void) const;
   void m_HidePopupFilter(void) const;
   void m_HidePopupLogging(void) const;
   void m_ShowPopupDatabase(const bool oq_Checked);
   void m_ShowPopupDllConfig(const bool oq_Checked);
   void m_ShowPopupFilter(const bool oq_Checked);
   void m_ShowPopupLogging(const bool oq_Checked);
   void m_ShowPopup(stw::opensyde_gui::C_CamMosSectionPopup * const opc_Popup,
                    const stw::opensyde_gui_elements::C_CamOgePubSettingsAdd * const opc_Button) const;
   QPoint m_GetPopupMovePoint(const stw::opensyde_gui_elements::C_CamOgePubSettingsAdd * const opc_Button) const;
   stw::opensyde_gui_logic::C_UsHandler::E_SettingsSubSection m_GetPopOpenIdentity(void) const;

   //Avoid call
   C_CamMosWidget(const C_CamMosWidget &);
   C_CamMosWidget & operator =(const C_CamMosWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
