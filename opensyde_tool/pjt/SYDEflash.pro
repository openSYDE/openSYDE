#-------------------------------------------------
#
# Project created by QtCreator 2016-05-03T11:00:30
#
#-------------------------------------------------

QT       += core gui
CONFIG   += precompile_header
CONFIG   += no_keywords

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets svg winextras

TARGET = SYDEflash
TEMPLATE = app

DESTDIR = ../result/utilities/SYDEflash

#version info:
RC_FILE = ../src/syde_flash/fla_resources.rc

#include relevant openSYDE core modules
opensyde_core_skip_modules += opensyde_core_skip_zipping
opensyde_core_skip_modules += opensyde_core_skip_code_generation
opensyde_core_skip_modules += opensyde_core_skip_protocol_drivers_system
opensyde_core_skip_modules += opensyde_core_skip_imports
include(../libs/opensyde_core/opensyde_core.pri)

SOURCES += \
    ../src/can_monitor/can_monitor_gui_elements/check_box/C_CamOgeChxSettings.cpp \
    ../src/can_monitor/can_monitor_gui_elements/combo_box/C_CamOgeCbxDark.cpp \
    ../src/can_monitor/can_monitor_gui_elements/combo_box/C_CamOgeCbxTable.cpp \
    ../src/can_monitor/can_monitor_gui_elements/combo_box/C_CamOgeCbxTableSmall.cpp \
    ../src/can_monitor/can_monitor_gui_elements/combo_box/C_CamOgeCbxWhite.cpp \
    ../src/can_monitor/can_monitor_gui_elements/group_box/C_CamOgeGbxMessageGeneratorBackground.cpp \
    ../src/can_monitor/can_monitor_gui_elements/group_box/C_CamOgeGbxMessageSignalsBorder.cpp \
    ../src/can_monitor/can_monitor_gui_elements/label/C_CamOgeLabStatusBar.cpp \
    ../src/can_monitor/can_monitor_gui_elements/line_edit/C_CamOgeLeIdPopUp.cpp \
    ../src/can_monitor/can_monitor_gui_elements/line_edit/C_CamOgeLeTableEdit.cpp \
    ../src/can_monitor/can_monitor_gui_elements/menu/C_CamOgeMuRecentProjects.cpp \
    ../src/can_monitor/can_monitor_gui_elements/push_button/C_CamOgePubAbout.cpp \
    ../src/can_monitor/can_monitor_gui_elements/push_button/C_CamOgePubDarkBrowse.cpp \
    ../src/can_monitor/can_monitor_gui_elements/push_button/C_CamOgePubPathVariablesBase.cpp \
    ../src/can_monitor/can_monitor_gui_elements/push_button/C_CamOgePubProjOp.cpp \
    ../src/can_monitor/can_monitor_gui_elements/push_button/C_CamOgePubSettingsAdd.cpp \
    ../src/can_monitor/can_monitor_gui_elements/spin_box/C_CamOgeSpxProperties.cpp \
    ../src/can_monitor/can_monitor_gui_elements/splitter/C_CamOgeSpi.cpp \
    ../src/can_monitor/can_monitor_gui_elements/tool_button/C_CamOgeTobRecentProjects.cpp \
    ../src/can_monitor/can_monitor_gui_elements/widget/C_CamOgeWiSectionHeader.cpp \
    ../src/can_monitor/can_monitor_gui_elements/widget/C_CamOgeWiSettingSubSection.cpp \
    ../src/can_monitor/can_monitor_gui_elements/widget/C_CamOgeWiSettingsBase.cpp \
    ../src/can_monitor/can_monitor_gui_elements/widget/C_CamOgeWiSpinBoxGroupTable.cpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosDllWidget.cpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosSectionPopup.cpp \
    ../src/can_monitor/util/C_CamUtiGeneric.cpp \
    ../src/gettext/C_GtGetText.cpp \
    ../src/help_engine/C_HeHandler.cpp \
    ../src/navigable_gui/C_NagAboutDialog.cpp \
    ../src/navigable_gui/C_NagToolTip.cpp \
    ../src/opensyde_gui_elements/C_OgeContextMenu.cpp \
    ../src/opensyde_gui_elements/C_OgeOverlay.cpp \
    ../src/opensyde_gui_elements/C_OgePopUpDialog.cpp \
    ../src/opensyde_gui_elements/C_OgeToolTipBase.cpp \
    ../src/opensyde_gui_elements/C_OgeTransparentScrollArea.cpp \
    ../src/opensyde_gui_elements/C_OgeTreeToolTipBase.cpp \
    ../src/opensyde_gui_elements/C_OgeTreeViewToolTipBase.cpp \
    ../src/opensyde_gui_elements/check_box/C_OgeChxProperties.cpp \
    ../src/opensyde_gui_elements/check_box/C_OgeChxToolTipBase.cpp \
    ../src/opensyde_gui_elements/check_box/C_OgeChxDefaultCheckedDisabled.cpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxIconDelegate.cpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxIconOnly.cpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxMultiSelect.cpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxResizingView.cpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxTableBase.cpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxText.cpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxToolTipBase.cpp \
    ../src/opensyde_gui_elements/frame/C_OgeFraBorder.cpp \
    ../src/opensyde_gui_elements/frame/C_OgeFraSeparator.cpp \
    ../src/opensyde_gui_elements/group_box/C_OgeGbx.cpp \
    ../src/opensyde_gui_elements/group_box/C_OgeGbxHover.cpp \
    ../src/opensyde_gui_elements/group_box/C_OgeGbxMouseMove.cpp \
    ../src/opensyde_gui_elements/group_box/C_OgeGbxToolTip.cpp \
    ../src/opensyde_gui_elements/group_box/C_OgeGbxToolTipBase.cpp \
    ../src/opensyde_gui_elements/group_box/C_OgeGbxTransparent.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabContextMenuBase.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabDescriptionMessage.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabElided.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabGenericBubble.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabGenericNoPaddingNoMargins.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabGroupItem.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabGroupItemValue.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabGroupSubItem.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabHeadingGroupBold.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabHeadingMessage.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabHeadingToolTip.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabPopUpSubTitle.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabPopUpTitle.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabPropertiesSubHeading.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabSvgOnly.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabToolTipBase.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabUpdateAppPath.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabUpdateAppVersion.cpp \
    ../src/opensyde_gui_elements/line_edit/C_OgeLeContextMenuBase.cpp \
    ../src/opensyde_gui_elements/line_edit/C_OgeLeDark.cpp \
    ../src/opensyde_gui_elements/line_edit/C_OgeLeFilePathBase.cpp \
    ../src/opensyde_gui_elements/line_edit/C_OgeLeProperties.cpp \
    ../src/opensyde_gui_elements/line_edit/C_OgeLePropertiesName.cpp \
    ../src/opensyde_gui_elements/line_edit/C_OgeLeToolTipBase.cpp \
    ../src/opensyde_gui_elements/menu/C_OgeMuSections.cpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubCancel.cpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubConfigure.cpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubDialog.cpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubIconOnly.cpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubIconText.cpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubMessageCancel.cpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubMessageOk.cpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubProjAction.cpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubStandard.cpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubSvgIconOnly.cpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubToolTipBase.cpp \
    ../src/opensyde_gui_elements/radio_button/C_OgeRabProperties.cpp \
    ../src/opensyde_gui_elements/radio_button/C_OgeRabToolTipBase.cpp \
    ../src/opensyde_gui_elements/scroll_area/C_OgeSaMain.cpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxAllBase.cpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxAutoFixBase.cpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxDoubleAutoFix.cpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxDoubleAutoFixCustomTrigger.cpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxDoubleDynamicDecimalsBase.cpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxDoubleToolTipBase.cpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxInt64.cpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxInt64AutoFix.cpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxInt64ToolTipBase.cpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxToolTipBase.cpp \
    ../src/opensyde_gui_elements/splitter/C_OgeSpiBase.cpp \
    ../src/opensyde_gui_elements/text_browser/C_OgeTebContextMenuBase.cpp \
    ../src/opensyde_gui_elements/text_browser/C_OgeTebMessageDetails.cpp \
    ../src/opensyde_gui_elements/text_edit/C_OgeTedContextMenuBase.cpp \
    ../src/opensyde_gui_elements/text_edit/C_OgeTedPropertiesComment.cpp \
    ../src/opensyde_gui_elements/text_edit/C_OgeTedToolTipBase.cpp \
    ../src/opensyde_gui_elements/text_edit/C_OgeTedToolTipContent.cpp \
    ../src/opensyde_gui_elements/tool_button/C_OgeTobToolTipBase.cpp \
    ../src/opensyde_gui_elements/widget/C_OgeWiCustomMessage.cpp \
    ../src/opensyde_gui_elements/widget/C_OgeWiError.cpp \
    ../src/opensyde_gui_elements/widget/C_OgeWiProgressBar.cpp \
    ../src/opensyde_gui_elements/widget/C_OgeWiOnlyBackground.cpp \
    ../src/opensyde_gui_elements/widget/C_OgeWiSpinBoxGroup.cpp \
    ../src/opensyde_gui_elements/widget/C_OgeWiUtil.cpp \
    ../src/opensyde_gui_elements/widget/C_OgeWiWithToolTip.cpp \
    ../src/project_operations/C_PopErrorHandling.cpp \
    ../src/syde_flash/C_FlaMainWindow.cpp \
    ../src/syde_flash/C_FlaTitleBarWidget.cpp \
    ../src/syde_flash/bottom_bar/C_FlaBottomBar.cpp \
    ../src/syde_flash/configure_node/C_FlaConNodeConfigPopup.cpp \
    ../src/syde_flash/fla_main.cpp \
    ../src/syde_flash/gui_elements/line_edit/C_FlaOgeLeFilePath.cpp \
    ../src/syde_flash/gui_elements/push_button/C_FlaOgePubPathVariables.cpp \
    ../src/syde_flash/gui_elements/spin_box/C_FlaOgeSpxDark.cpp \
    ../src/syde_flash/gui_elements/text_browser/C_FlaOgeTebProgressLog.cpp \
    ../src/syde_flash/properties/C_FlaPropWidget.cpp \
    ../src/syde_flash/search_nodes/C_FlaSenDcBasicSequences.cpp \
    ../src/syde_flash/search_nodes/C_FlaSenSearchNodePopup.cpp \
    ../src/syde_flash/settings/C_FlaDllWidget.cpp \
    ../src/syde_flash/settings/C_FlaSetAdvancedPropertiesWidget.cpp \
    ../src/syde_flash/settings/C_FlaSetProgressWidget.cpp \
    ../src/syde_flash/settings/C_FlaSetWidget.cpp \
    ../src/syde_flash/update/C_FlaUpHexFileInfo.cpp \
    ../src/syde_flash/update/C_FlaUpHexFileView.cpp \
    ../src/syde_flash/update/C_FlaUpListItemWidget.cpp \
    ../src/syde_flash/update/C_FlaUpListWidget.cpp \
    ../src/syde_flash/update/C_FlaUpProperties.cpp \
    ../src/syde_flash/update/C_FlaUpSequences.cpp \
    ../src/syde_flash/user_settings/C_UsFiler.cpp \
    ../src/syde_flash/user_settings/C_UsHandler.cpp \
    ../src/syde_flash/util/C_FlaUti.cpp \
    ../src/syde_flash/util/C_FlaUtiStyleSheets.cpp \
    ../src/system_definition/C_SdTooltipUtil.cpp \
    ../src/system_definition/node_edit/datapools/C_SdNdeDpContentUtil.cpp\
    ../src/system_views/communication/C_SyvComDriverThread.cpp \
    ../src/system_views/device_configuration/C_SyvDcConnectedNodeList.cpp \
    ../src/system_views/device_configuration/C_SyvDcConnectedNodeWidget.cpp \
    ../src/table_base/C_TblDelegate.cpp \
    ../src/table_base/C_TblEditLineEditBase.cpp \
    ../src/table_base/C_TblModelAction.cpp \
    ../src/table_base/C_TblViewInteraction.cpp \
    ../src/table_base/C_TblViewScroll.cpp \
    ../src/table_base/C_TblViewToolTipBase.cpp \
    ../src/table_base/tree_base/C_TblTreDataElementSortFilter.cpp \
    ../src/table_base/tree_base/C_TblTreItem.cpp \
    ../src/table_base/tree_base/C_TblTreModel.cpp \
    ../src/table_base/tree_base/C_TblTreSimpleItem.cpp \
    ../src/table_base/tree_base/C_TblTreSimpleModel.cpp \
    ../src/util/C_Uti.cpp \
    ../src/util/C_UtiClipBoardHelper.cpp \
    ../src/util/C_UtiFindNameHelper.cpp

PRECOMPILED_HEADER = ../src/precompiled_headers/syde_flash/precomp_headers.hpp

HEADERS  += \
    ../src/can_monitor/can_monitor_gui_elements/check_box/C_CamOgeChxSettings.hpp \
    ../src/can_monitor/can_monitor_gui_elements/combo_box/C_CamOgeCbxDark.hpp \
    ../src/can_monitor/can_monitor_gui_elements/combo_box/C_CamOgeCbxTable.hpp \
    ../src/can_monitor/can_monitor_gui_elements/combo_box/C_CamOgeCbxTableSmall.hpp \
    ../src/can_monitor/can_monitor_gui_elements/combo_box/C_CamOgeCbxWhite.hpp \
    ../src/can_monitor/can_monitor_gui_elements/group_box/C_CamOgeGbxMessageGeneratorBackground.hpp \
    ../src/can_monitor/can_monitor_gui_elements/group_box/C_CamOgeGbxMessageSignalsBorder.hpp \
    ../src/can_monitor/can_monitor_gui_elements/label/C_CamOgeLabStatusBar.hpp \
    ../src/can_monitor/can_monitor_gui_elements/line_edit/C_CamOgeLeIdPopUp.hpp \
    ../src/can_monitor/can_monitor_gui_elements/line_edit/C_CamOgeLeTableEdit.hpp \
    ../src/can_monitor/can_monitor_gui_elements/menu/C_CamOgeMuRecentProjects.hpp \
    ../src/can_monitor/can_monitor_gui_elements/push_button/C_CamOgePubAbout.hpp \
    ../src/can_monitor/can_monitor_gui_elements/push_button/C_CamOgePubDarkBrowse.hpp \
    ../src/can_monitor/can_monitor_gui_elements/push_button/C_CamOgePubPathVariablesBase.hpp \
    ../src/can_monitor/can_monitor_gui_elements/push_button/C_CamOgePubProjOp.hpp \
    ../src/can_monitor/can_monitor_gui_elements/push_button/C_CamOgePubSettingsAdd.hpp \
    ../src/can_monitor/can_monitor_gui_elements/spin_box/C_CamOgeSpxProperties.hpp \
    ../src/can_monitor/can_monitor_gui_elements/splitter/C_CamOgeSpi.hpp \
    ../src/can_monitor/can_monitor_gui_elements/tool_button/C_CamOgeTobRecentProjects.hpp \
    ../src/can_monitor/can_monitor_gui_elements/widget/C_CamOgeWiSectionHeader.hpp \
    ../src/can_monitor/can_monitor_gui_elements/widget/C_CamOgeWiSettingSubSection.hpp \
    ../src/can_monitor/can_monitor_gui_elements/widget/C_CamOgeWiSettingsBase.hpp \
    ../src/can_monitor/can_monitor_gui_elements/widget/C_CamOgeWiSpinBoxGroupTable.hpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosDllWidget.hpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosSectionPopup.hpp \
    ../src/can_monitor/util/C_CamUtiGeneric.hpp \
    ../src/constants.hpp \
    ../src/gettext/C_GtGetText.hpp \
    ../src/help_engine/C_HeHandler.hpp \
    ../src/navigable_gui/C_NagAboutDialog.hpp \
    ../src/navigable_gui/C_NagToolTip.hpp \
    ../src/opensyde_gui_elements/C_OgeContextMenu.hpp \
    ../src/opensyde_gui_elements/C_OgeOverlay.hpp \
    ../src/opensyde_gui_elements/C_OgePopUpDialog.hpp \
    ../src/opensyde_gui_elements/C_OgeToolTipBase.hpp \
    ../src/opensyde_gui_elements/C_OgeTransparentScrollArea.hpp \
    ../src/opensyde_gui_elements/C_OgeTreeToolTipBase.hpp \
    ../src/opensyde_gui_elements/C_OgeTreeViewToolTipBase.hpp \
    ../src/opensyde_gui_elements/check_box/C_OgeChxProperties.hpp \
    ../src/opensyde_gui_elements/check_box/C_OgeChxToolTipBase.hpp \
    ../src/opensyde_gui_elements/check_box/C_OgeChxDefaultCheckedDisabled.hpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxIconDelegate.hpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxIconOnly.hpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxMultiSelect.hpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxResizingView.hpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxTableBase.hpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxText.hpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxToolTipBase.hpp \
    ../src/opensyde_gui_elements/frame/C_OgeFraBorder.hpp \
    ../src/opensyde_gui_elements/frame/C_OgeFraSeparator.hpp \
    ../src/opensyde_gui_elements/group_box/C_OgeGbx.hpp \
    ../src/opensyde_gui_elements/group_box/C_OgeGbxHover.hpp \
    ../src/opensyde_gui_elements/group_box/C_OgeGbxMouseMove.hpp \
    ../src/opensyde_gui_elements/group_box/C_OgeGbxToolTip.hpp \
    ../src/opensyde_gui_elements/group_box/C_OgeGbxToolTipBase.hpp \
    ../src/opensyde_gui_elements/group_box/C_OgeGbxTransparent.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabContextMenuBase.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabDescriptionMessage.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabElided.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabGenericBubble.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabGenericNoPaddingNoMargins.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabGroupItem.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabGroupItemValue.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabGroupSubItem.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabHeadingGroupBold.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabHeadingMessage.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabHeadingToolTip.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabPopUpSubTitle.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabPopUpTitle.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabPropertiesSubHeading.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabSvgOnly.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabToolTipBase.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabUpdateAppPath.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabUpdateAppVersion.hpp \
    ../src/opensyde_gui_elements/line_edit/C_OgeLeContextMenuBase.hpp \
    ../src/opensyde_gui_elements/line_edit/C_OgeLeDark.hpp \
    ../src/opensyde_gui_elements/line_edit/C_OgeLeFilePathBase.hpp \
    ../src/opensyde_gui_elements/line_edit/C_OgeLeProperties.hpp \
    ../src/opensyde_gui_elements/line_edit/C_OgeLePropertiesName.hpp \
    ../src/opensyde_gui_elements/line_edit/C_OgeLeToolTipBase.hpp \
    ../src/opensyde_gui_elements/menu/C_OgeMuSections.hpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubCancel.hpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubConfigure.hpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubDialog.hpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubIconOnly.hpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubIconText.hpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubMessageCancel.hpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubMessageOk.hpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubProjAction.hpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubStandard.hpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubSvgIconOnly.hpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubToolTipBase.hpp \
    ../src/opensyde_gui_elements/radio_button/C_OgeRabProperties.hpp \
    ../src/opensyde_gui_elements/radio_button/C_OgeRabToolTipBase.hpp \
    ../src/opensyde_gui_elements/scroll_area/C_OgeSaMain.hpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxAllBase.hpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxAutoFixBase.hpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxDoubleAutoFix.hpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxDoubleAutoFixCustomTrigger.hpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxDoubleDynamicDecimalsBase.hpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxDoubleToolTipBase.hpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxInt64.hpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxInt64AutoFix.hpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxInt64ToolTipBase.hpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxToolTipBase.hpp \
    ../src/opensyde_gui_elements/splitter/C_OgeSpiBase.hpp \
    ../src/opensyde_gui_elements/text_browser/C_OgeTebContextMenuBase.hpp \
    ../src/opensyde_gui_elements/text_browser/C_OgeTebMessageDetails.hpp \
    ../src/opensyde_gui_elements/text_edit/C_OgeTedContextMenuBase.hpp \
    ../src/opensyde_gui_elements/text_edit/C_OgeTedPropertiesComment.hpp \
    ../src/opensyde_gui_elements/text_edit/C_OgeTedToolTipBase.hpp \
    ../src/opensyde_gui_elements/text_edit/C_OgeTedToolTipContent.hpp \
    ../src/opensyde_gui_elements/tool_button/C_OgeTobToolTipBase.hpp \
    ../src/opensyde_gui_elements/widget/C_OgeWiCustomMessage.hpp \
    ../src/opensyde_gui_elements/widget/C_OgeWiError.hpp \
    ../src/opensyde_gui_elements/widget/C_OgeWiProgressBar.hpp \
    ../src/opensyde_gui_elements/widget/C_OgeWiOnlyBackground.hpp \
    ../src/opensyde_gui_elements/widget/C_OgeWiSpinBoxGroup.hpp \
    ../src/opensyde_gui_elements/widget/C_OgeWiUtil.hpp \
    ../src/opensyde_gui_elements/widget/C_OgeWiWithToolTip.hpp \
    ../src/project_operations/C_PopErrorHandling.hpp \
    ../src/syde_flash/C_FlaMainWindow.hpp \
    ../src/syde_flash/C_FlaTitleBarWidget.hpp \
    ../src/syde_flash/bottom_bar/C_FlaBottomBar.hpp \
    ../src/syde_flash/configure_node/C_FlaConNodeConfigPopup.hpp \
    ../src/syde_flash/fla_constants.hpp \
    ../src/syde_flash/gui_elements/line_edit/C_FlaOgeLeFilePath.hpp \
    ../src/syde_flash/gui_elements/push_button/C_FlaOgePubPathVariables.hpp \
    ../src/syde_flash/gui_elements/spin_box/C_FlaOgeSpxDark.hpp \
    ../src/syde_flash/gui_elements/text_browser/C_FlaOgeTebProgressLog.hpp \
    ../src/syde_flash/properties/C_FlaPropWidget.hpp \
    ../src/syde_flash/search_nodes/C_FlaSenDcBasicSequences.hpp \
    ../src/syde_flash/search_nodes/C_FlaSenSearchNodePopup.hpp \
    ../src/syde_flash/settings/C_FlaDllWidget.hpp \
    ../src/syde_flash/settings/C_FlaSetAdvancedPropertiesWidget.hpp \
    ../src/syde_flash/settings/C_FlaSetProgressWidget.hpp \
    ../src/syde_flash/settings/C_FlaSetWidget.hpp \
    ../src/syde_flash/update/C_FlaUpHexFileInfo.hpp \
    ../src/syde_flash/update/C_FlaUpHexFileView.hpp \
    ../src/syde_flash/update/C_FlaUpListItemWidget.hpp \
    ../src/syde_flash/update/C_FlaUpListWidget.hpp \
    ../src/syde_flash/update/C_FlaUpProperties.hpp \
    ../src/syde_flash/update/C_FlaUpSequences.hpp \
    ../src/syde_flash/user_settings/C_UsFiler.hpp \
    ../src/syde_flash/user_settings/C_UsHandler.hpp \
    ../src/syde_flash/util/C_FlaUti.hpp \
    ../src/syde_flash/util/C_FlaUtiStyleSheets.hpp \
    ../src/system_definition/C_SdTooltipUtil.hpp \
    ../src/system_definition/node_edit/datapools/C_SdNdeDpContentUtil.hpp \
    ../src/system_views/communication/C_SyvComDriverThread.hpp \
    ../src/system_views/device_configuration/C_SyvDcConnectedNodeList.hpp \
    ../src/system_views/device_configuration/C_SyvDcConnectedNodeWidget.hpp \
    ../src/table_base/C_TblDelegate.hpp \
    ../src/table_base/C_TblEditLineEditBase.hpp \
    ../src/table_base/C_TblModelAction.hpp \
    ../src/table_base/C_TblViewInteraction.hpp \
    ../src/table_base/C_TblViewScroll.hpp \
    ../src/table_base/C_TblViewToolTipBase.hpp \
    ../src/table_base/tree_base/C_TblTreDataElementSortFilter.hpp \
    ../src/table_base/tree_base/C_TblTreItem.hpp \
    ../src/table_base/tree_base/C_TblTreModel.hpp \
    ../src/table_base/tree_base/C_TblTreSimpleItem.hpp \
    ../src/table_base/tree_base/C_TblTreSimpleModel.hpp \
    ../src/util/C_Uti.hpp \
    ../src/util/C_UtiClipBoardHelper.hpp \
    ../src/util/C_UtiFindNameHelper.hpp

FORMS    += \
    ../src/can_monitor/can_monitor_gui_elements/widget/C_CamOgeWiSectionHeader.ui \
    ../src/can_monitor/can_monitor_gui_elements/widget/C_CamOgeWiSettingSubSection.ui \
    ../src/can_monitor/can_monitor_settings/C_CamMosDllWidget.ui \
    ../src/navigable_gui/C_NagAboutDialog.ui \
    ../src/navigable_gui/C_NagToolTip.ui \
    ../src/opensyde_gui_elements/C_OgePopUpDialog.ui \
    ../src/opensyde_gui_elements/widget/C_OgeWiCustomMessage.ui \
    ../src/opensyde_gui_elements/widget/C_OgeWiError.ui \
    ../src/opensyde_gui_elements/widget/C_OgeWiSpinBoxGroup.ui \
    ../src/syde_flash/C_FlaMainWindow.ui \
    ../src/syde_flash/C_FlaTitleBarWidget.ui \
    ../src/syde_flash/bottom_bar/C_FlaBottomBar.ui \
    ../src/syde_flash/configure_node/C_FlaConNodeConfigPopup.ui \
    ../src/syde_flash/properties/C_FlaPropWidget.ui \
    ../src/syde_flash/search_nodes/C_FlaSenSearchNodePopup.ui \
    ../src/syde_flash/settings/C_FlaSetAdvancedPropertiesWidget.ui \
    ../src/syde_flash/settings/C_FlaSetProgressWidget.ui \
    ../src/syde_flash/settings/C_FlaSetWidget.ui \
    ../src/syde_flash/update/C_FlaUpHexFileView.ui \
    ../src/syde_flash/update/C_FlaUpListItemWidget.ui \
    ../src/syde_flash/update/C_FlaUpProperties.ui \
    ../src/system_views/device_configuration/C_SyvDcConnectedNodeWidget.ui

INCLUDEPATH += \
               ../libs/gettext \
               ../src/syde_flash/gui_elements/line_edit \
               ../src/syde_flash/gui_elements/push_button \
               ../src/syde_flash/gui_elements/spin_box \
               ../src/syde_flash/gui_elements/text_browser \
               ../src/can_monitor/can_monitor_gui_elements/check_box \
               ../src/can_monitor/can_monitor_gui_elements/combo_box \
               ../src/can_monitor/can_monitor_gui_elements/group_box \
               ../src/can_monitor/can_monitor_gui_elements/label \
               ../src/can_monitor/can_monitor_gui_elements/line_edit \
               ../src/can_monitor/can_monitor_gui_elements/menu \
               ../src/can_monitor/can_monitor_gui_elements/push_button \
               ../src/can_monitor/can_monitor_gui_elements/spin_box \
               ../src/can_monitor/can_monitor_gui_elements/splitter \
               ../src/can_monitor/can_monitor_gui_elements/tool_button \
               ../src/can_monitor/can_monitor_gui_elements/widget \
               ../src/can_monitor/can_monitor_settings \
               ../src/can_monitor/util \
               ../src/gettext \
               ../src/gettext \
               ../src/help_engine \
               ../src/navigable_gui \
               ../src/opensyde_gui_elements \
               ../src/opensyde_gui_elements/check_box \
               ../src/opensyde_gui_elements/combo_box \
               ../src/opensyde_gui_elements/frame \
               ../src/opensyde_gui_elements/group_box \
               ../src/opensyde_gui_elements/label \
               ../src/opensyde_gui_elements/line_edit \
               ../src/opensyde_gui_elements/menu \
               ../src/opensyde_gui_elements/push_button \
               ../src/opensyde_gui_elements/radio_button \
               ../src/opensyde_gui_elements/scroll_area \
               ../src/opensyde_gui_elements/spin_box \
               ../src/opensyde_gui_elements/splitter \
               ../src/opensyde_gui_elements/text_browser \
               ../src/opensyde_gui_elements/text_edit \
               ../src/opensyde_gui_elements/tool_button \
               ../src/opensyde_gui_elements/widget \
               ../src/precompiled_headers/can_monitor \
               ../src/precompiled_headers/syde_flash \
               ../src/project_operations/ \
               ../src/syde_flash \
               ../src/syde_flash/bottom_bar \
               ../src/syde_flash/configure_node \
               ../src/syde_flash/properties \
               ../src/syde_flash/search_nodes \
               ../src/syde_flash/settings \
               ../src/syde_flash/update \
               ../src/syde_flash/user_settings \
               ../src/syde_flash/util \
               ../src/system_definition \
               ../src/system_definition/node_edit/datapools \
               ../src/system_views/communication \
               ../src/system_views/device_configuration \
               ../src/table_base \
               ../src/table_base/tree_base \
               ../src/util \
               ../src

#do not issue deprecation warnings (tested code can contain deprecated functions which we do want to provide)
win32-g++ {
QMAKE_CXXFLAGS += -Wno-deprecated-declarations
}

LIBS += -L../libs/gettext -lintl \
        -lz

LIBS += -lws2_32   #WinSock
LIBS += -lIphlpapi #IP helper API

#add windows API libraries
LIBS += -lversion

QMAKE_TARGET_COMPANY = STW
QMAKE_TARGET_PRODUCT = SYDEflash
#QMAKE_TARGET_DESCRIPTION = TODO
#See C coding rules v4.00r0
QMAKE_TARGET_COPYRIGHT = STW (c) 2018

DISTFILES +=

RESOURCES += \
    ../src/syde_flash/fla_application.qrc

#compiler with function intrumentation (for "Embedded Profiler")
#QMAKE_CXXFLAGS += -finstrument-functions
#workaround for https://bugreports.qt.io/browse/QTBUG-35884 (for "Embedded Profiler")
#QMAKE_CXXFLAGS += -finstrument-functions-exclude-function-list=load,store
#create map file
#QMAKE_LFLAGS += -Wl,-Map=qt_test.map
#libraries for "Embedded Profiler"
#LIBS += -L$$PWD/eprofiler-windows32-mingw-intel-1.2.3/EProfiler/windows32-mingw-intel/lib/ -lEProfilerStatic
#LIBS += -lpthread
