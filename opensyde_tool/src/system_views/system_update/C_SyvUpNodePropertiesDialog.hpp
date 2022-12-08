//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Dialog for system update node properties view (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SYVUPNODEPROPERTIESDIALOG_H
#define C_SYVUPNODEPROPERTIESDIALOG_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.hpp"
#include "C_SyvUtil.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_SyvUpDeviceInfo.hpp"
#include "C_GiSvNodeData.hpp"
#include "C_PuiSvNodeUpdateParamInfo.hpp"

namespace Ui
{
class C_SyvUpNodePropertiesDialog;
}
/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SyvUpNodePropertiesDialog :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SyvUpNodePropertiesDialog(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                        const uint32_t ou32_NodeIndex, const C_GiSvNodeData & orc_NodeData);
   ~C_SyvUpNodePropertiesDialog(void) override;

   void InitStaticNames(void) const;
   void SetStatus(const C_GiSvNodeData & orc_NodeData);
   void CopyInitialStatus(C_GiSvNodeData & orc_NodeData) const;
   void CopyUpdateStatus(C_GiSvNodeData & orc_NodeData) const;
   void CopyStwDeviceInfo(C_GiSvNodeData & orc_NodeData) const;
   void CopyOpenSydeDeviceInfo(C_GiSvNodeData & orc_NodeData) const;
   void CopyDiscardedStatus(C_GiSvNodeData & orc_NodeData) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   Ui::C_SyvUpNodePropertiesDialog * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
   const uint32_t mu32_NodeIndex;

   C_GiSvNodeData mc_NodeData;

   static const QString mhc_HTML_CELL_TAG_START_MAIN;
   static const QString mhc_HTML_CELL_TAG_START_CONTENT;
   static const QString mhc_HTML_CELL_TAG_START_STATUS;

   void m_OkClicked(void);
   void m_InitStatus(void) const;
   void m_InitDataBlockTable(void) const;
   static void mh_InitDataBlockTableForNode(const C_GiSvSubNodeData & orc_NodeInfo, QString & orc_Content,
                                            const stw::opensyde_core::C_OscNode & orc_Node,
                                            const bool oq_IsMultiDevice);
   static void mh_GetApplicationDataForNode(const C_GiSvSubNodeData & orc_NodeInfo,
                                            const stw::opensyde_core::C_OscNode & orc_Node,
                                            const uint32_t ou32_ApplicationIndex, QString & orc_ApplicationName,
                                            QString & orc_ApplicationStateIcon, QString & orc_ApplicationState,
                                            QString & orc_DeviceValidStatus, QString & orc_FileProjectName,
                                            QString & orc_DeviceProjectName, QString & orc_FileVersion,
                                            QString & orc_DeviceFileVersion, QString & orc_FileBuildDate,
                                            QString & orc_DeviceBuildDate);
   static void mh_ExtractStwDeviceInformation(const C_GiSvSubNodeData & orc_NodeInfo,
                                              const QString & orc_FileProjectName, QString & orc_DeviceProjectName,
                                              const QString & orc_FileVersion, QString & orc_DeviceFileVersion,
                                              const QString & orc_FileBuildDate, QString & orc_DeviceBuildDate,
                                              QString & orc_DeviceValidStatus, bool & orq_MissingStatus,
                                              bool & orq_MatchStatus);
   static void mh_ExtractOpenSydeDeviceInformation(const C_GiSvSubNodeData & orc_NodeInfo,
                                                   const QString & orc_FileProjectName, QString & orc_DeviceProjectName,
                                                   const QString & orc_FileVersion, QString & orc_DeviceFileVersion,
                                                   const QString & orc_FileBuildDate, QString & orc_DeviceBuildDate,
                                                   QString & orc_DeviceValidStatus, bool & orq_MissingStatus,
                                                   bool & orq_MatchStatus);
   static bool mh_HandleHighlighting(const QString & orc_FileProjectName, QString & orc_DeviceProjectName,
                                     const QString & orc_FileVersion, QString & orc_DeviceFileVersion,
                                     const QString & orc_FileBuildDate, QString & orc_DeviceBuildDate);
   static void mh_ExtractDetailsPartFromDataForNode(const bool oq_HexAppInfoAmbiguous,
                                                    const QString & orc_FileProjectName,
                                                    const QString & orc_DeviceProjectName,
                                                    const QString & orc_FileVersion,
                                                    const QString & orc_DeviceFileVersion,
                                                    const QString & orc_FileBuildDate,
                                                    const QString & orc_DeviceBuildDate, QString & orc_Content);
   static void mh_AppendApplicationForNode(const uint32_t ou32_ApplicationIndex, const QString & orc_ApplicationName,
                                           const QString & orc_ApplicationStateIcon,
                                           const QString & orc_ApplicationState, const QString & orc_DeviceValidStatus,
                                           const QString & orc_DetailsPart, QString & orc_Content);
   static void mh_InitDataBlockTableOtherSectionForNode(const C_GiSvSubNodeData & orc_NodeInfo, QString & orc_Content);
   void m_InitFlashloaderTable(void) const;
   static void mh_InitFlashloaderTableForNode(const C_GiSvSubNodeData & orc_NodeInfo, QString & orc_Content,
                                              const stw::opensyde_core::C_OscNode & orc_Node,
                                              const bool oq_IsMultiDevice);
   static void mh_HandleSectionAppend(const QString & orc_NewPart, QString & orc_CompleteSection,
                                      const stw::opensyde_core::C_OscNode & orc_Node, const bool oq_IsMultiDevice);
   void m_OnDiscard(void);
   void m_ReInitStatus(void) const;
   void m_UpdateProgressLog(void) const;
   QString m_GetProgressLogConnectStatesString(void) const;
   static QString mh_GetProgressLogConnectStatesStringForSubNode(const C_GiSvSubNodeData * const opc_SubNodeData);
   QString m_GetProgressLogUpdateStatesString(void) const;
   static QString mh_GetProgressLogUpdateStatesStringForSubNode(const C_GiSvSubNodeData * const opc_SubNodeData);

   static QString mh_GetTableLineForBrowser(const QString & orc_FirstRow, const QString & orc_SecondRow,
                                            const uint32_t ou32_Layer);
   static QString mh_GetSuSequenceNodeStateString(const stw::opensyde_core::E_OscSuSequencesNodeState oe_State);
   static QString mh_GetSuSequenceNodeFileLoadStateString(
      const stw::opensyde_core::E_OscSuSequencesNodeState oe_StateExists,
      const stw::opensyde_core::E_OscSuSequencesNodeState oe_StateLoadedd);
   static QString mh_GetSuSequenceNodePreconditionString(const bool oq_FeatureSupported, const bool oq_FeatureError);

   //Avoid call
   C_SyvUpNodePropertiesDialog(const C_SyvUpNodePropertiesDialog &);
   C_SyvUpNodePropertiesDialog & operator =(const C_SyvUpNodePropertiesDialog &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
