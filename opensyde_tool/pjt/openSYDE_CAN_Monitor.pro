#-------------------------------------------------
#
# Project created by QtCreator 2016-05-03T11:00:30
#
#-------------------------------------------------

QT       += core gui
CONFIG   += precompile_header
CONFIG   += no_keywords

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets svg

TARGET = openSYDE_CAN_Monitor
TEMPLATE = app

DESTDIR = ../result/tool/CAN_Monitor

#version info:
RC_FILE = ../src/can_monitor/cam_resources.rc

#include relevant openSYDE core modules
opensyde_core_skip_modules += opensyde_core_skip_zipping
opensyde_core_skip_modules += opensyde_core_skip_code_generation
opensyde_core_skip_modules += opensyde_core_skip_protocol_drivers
include(../libs/opensyde_core/opensyde_core.pri)

SOURCES += \
    ../src/gettext/C_GtGetText.cpp \
    ../src/util/C_Uti.cpp \
    ../src/system_views/communication/C_SyvComMessageMonitor.cpp \
    ../src/system_views/communication/C_SyvComMessageLoggerFileBlf.cpp \
    ../src/system_views/communication/C_SyvComDriverThread.cpp \
    ../src/com_import_export/C_CieImportDbc.cpp \
    ../src/system_definition/node_edit/datapools/C_SdNdeDpContentUtil.cpp\
    ../src/can_monitor/cam_main.cpp \
    ../src/project_operations/C_PopErrorHandling.cpp \
    ../libs/dbc_driver_library/src/Vector/DBC/Attribute.cpp \
    ../libs/dbc_driver_library/src/Vector/DBC/AttributeDefinition.cpp \
    ../libs/dbc_driver_library/src/Vector/DBC/AttributeRelation.cpp \
    ../libs/dbc_driver_library/src/Vector/DBC/BitTiming.cpp \
    ../libs/dbc_driver_library/src/Vector/DBC/EnvironmentVariable.cpp \
    ../libs/dbc_driver_library/src/Vector/DBC/ExtendedMultiplexor.cpp \
    ../libs/dbc_driver_library/src/Vector/DBC/File.cpp \
    ../libs/dbc_driver_library/src/Vector/DBC/FileLoad.cpp \
    ../libs/dbc_driver_library/src/Vector/DBC/FileSave.cpp \
    ../libs/dbc_driver_library/src/Vector/DBC/Message.cpp \
    ../libs/dbc_driver_library/src/Vector/DBC/Network.cpp \
    ../libs/dbc_driver_library/src/Vector/DBC/Node.cpp \
    ../libs/dbc_driver_library/src/Vector/DBC/Signal.cpp \
    ../libs/dbc_driver_library/src/Vector/DBC/SignalGroup.cpp \
    ../libs/dbc_driver_library/src/Vector/DBC/SignalType.cpp \
    ../libs/dbc_driver_library/src/Vector/DBC/ValueTable.cpp \
    ../src/opensyde_gui_elements/widget/C_OgeWiCustomMessage.cpp \
    ../src/opensyde_gui_elements/widget/C_OgeWiError.cpp \
    ../src/opensyde_gui_elements/C_OgePopUpDialog.cpp \
    ../src/opensyde_gui_elements/C_OgeOverlay.cpp \
    ../src/opensyde_gui_elements/group_box/C_OgeGbxMouseMove.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabHeadingMessage.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabDescriptionMessage.cpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubCancel.cpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubDialog.cpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubIconText.cpp \
    ../src/opensyde_gui_elements/text_browser/C_OgeTebMessageDetails.cpp \
    ../src/navigable_gui/C_NagToolTip.cpp \
    ../src/opensyde_gui_elements/widget/C_OgeWiUtil.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabPopUpTitle.cpp \
    ../src/help_engine/C_HeHandler.cpp \
    ../src/opensyde_gui_elements/group_box/C_OgeGbxHover.cpp \
    ../src/opensyde_gui_elements/group_box/C_OgeGbxTransparent.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabContextMenuBase.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabPopUpSubTitle.cpp \
    ../src/opensyde_gui_elements/frame/C_OgeFraBorder.cpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubStandard.cpp \
    ../src/opensyde_gui_elements/text_browser/C_OgeTebContextMenuBase.cpp \
    ../src/opensyde_gui_elements/group_box/C_OgeGbxToolTip.cpp \
    ../src/opensyde_gui_elements/group_box/C_OgeGbxToolTipBase.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabHeadingToolTip.cpp \
    ../src/opensyde_gui_elements/text_edit/C_OgeTedToolTipContent.cpp \
    ../src/opensyde_gui_elements/C_OgeContextMenu.cpp \
    ../src/opensyde_gui_elements/C_OgeToolTipBase.cpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubToolTipBase.cpp \
    ../src/can_monitor/C_CamMainWindow.cpp \
    ../src/opensyde_gui_elements/widget/C_OgeWiOnlyBackground.cpp \
    ../src/opensyde_gui_elements/widget/C_OgeWiWithToolTip.cpp \
    ../src/can_monitor/message_trace/C_CamMetTreeDelegate.cpp \
    ../src/can_monitor/message_trace/C_CamMetTreeGuiBuffer.cpp \
    ../src/can_monitor/message_trace/C_CamMetTreeModel.cpp \
    ../src/can_monitor/message_trace/C_CamMetTreeView.cpp \
    ../src/table_base/tree_base/C_TblTreItem.cpp \
    ../src/table_base/tree_base/C_TblTreModel.cpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubSvgIconOnly.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabSvgOnly.cpp \
    ../src/can_monitor/user_settings/C_UsHandler.cpp \
    ../src/can_monitor/user_settings/C_UsFiler.cpp \
    ../src/navigable_gui/C_NagAboutDialog.cpp \
    ../src/opensyde_gui_elements/group_box/C_OgeGbx.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabGroupItem.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabGroupItemValue.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabToolTipBase.cpp \
    ../src/can_monitor/C_CamComDriverBase.cpp \
    ../src/can_monitor/message_trace/C_CamMetWidget.cpp \
    ../src/can_monitor/message_trace/C_CamMetStatusBarWidget.cpp \
    ../src/can_monitor/message_trace/C_CamMetControlBarWidget.cpp \
    ../src/can_monitor/message_generator/C_CamGenWidget.cpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosWidget.cpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosDllWidget.cpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosFilterWidget.cpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosDatabaseWidget.cpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosLoggingWidget.cpp \
    ../src/can_monitor/C_CamTitleBarWidget.cpp \
    ../src/can_monitor/util/C_CamUtiStyleSheets.cpp \
    ../src/can_monitor/can_monitor_gui_elements/label/C_CamOgeLabStatusBar.cpp \
    ../src/util/C_UtiClipBoardHelper.cpp \
    ../src/can_monitor/message_trace/C_CamMetClipBoardHelper.cpp \
    ../src/can_monitor/can_monitor_gui_elements/splitter/C_CamOgeSpi.cpp \
    ../src/opensyde_gui_elements/splitter/C_OgeSpiBase.cpp \
    ../src/can_monitor/can_monitor_gui_elements/widget/C_CamOgeWiSectionHeader.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabGenericNoPaddingNoMargins.cpp \
    ../src/can_monitor/can_monitor_gui_elements/widget/C_CamOgeWiSettingSubSection.cpp \
    ../src/opensyde_gui_elements/radio_button/C_OgeRabProperties.cpp \
    ../src/opensyde_gui_elements/radio_button/C_OgeRabToolTipBase.cpp \
    ../src/opensyde_gui_elements/frame/C_OgeFraSeparator.cpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubProjAction.cpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxToolTipBase.cpp \
    ../src/can_monitor/can_monitor_gui_elements/combo_box/C_CamOgeCbxWhite.cpp \
    ../src/can_monitor/can_monitor_gui_elements/push_button/C_CamOgePubAbout.cpp \
    ../src/can_monitor/message_generator/C_CamGenTableView.cpp \
    ../src/can_monitor/message_generator/C_CamGenTableModel.cpp \
    ../src/opensyde_gui_elements/line_edit/C_OgeLeContextMenuBase.cpp \
    ../src/opensyde_gui_elements/line_edit/C_OgeLeDark.cpp \
    ../src/util/C_UtiFindNameHelper.cpp \
    ../src/can_monitor/message_generator/C_CamGenMessagesWidget.cpp \
    ../src/can_monitor/project/C_CamProHandler.cpp \
    ../src/can_monitor/project/C_CamProHandlerFiler.cpp \
    ../src/can_monitor/project/C_CamProMessageData.cpp \
    ../src/can_monitor/can_monitor_gui_elements/push_button/C_CamOgePubProjOp.cpp \
    ../src/can_monitor/project/C_CamProClipBoardHelper.cpp \
    ../src/table_base/C_TblModelAction.cpp \
    ../src/can_monitor/can_monitor_gui_elements/push_button/C_CamOgePubSettingsAdd.cpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosFilterItemWidget.cpp \
    ../src/opensyde_gui_elements/C_OgeTransparentScrollArea.cpp \
    ../src/can_monitor/can_monitor_gui_elements/check_box/C_CamOgeChxSettings.cpp \
    ../src/opensyde_gui_elements/check_box/C_OgeChxToolTipBase.cpp \
    ../src/table_base/C_TblViewScroll.cpp \
    ../src/table_base/C_TblViewToolTipBase.cpp \
    ../src/table_base/C_TblDelegate.cpp \
    ../src/opensyde_gui_elements/line_edit/C_OgeLeToolTipBase.cpp \
    ../src/can_monitor/can_monitor_gui_elements/combo_box/C_CamOgeCbxTable.cpp \
    ../src/can_monitor/can_monitor_gui_elements/line_edit/C_CamOgeLeTableEdit.cpp \
    ../src/table_base/C_TblEditLineEditBase.cpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxIconDelegate.cpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxResizingView.cpp \
    ../src/can_monitor/can_monitor_gui_elements/group_box/C_CamOgeGbxMessageGeneratorBackground.cpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosFilterPopup.cpp \
    ../src/opensyde_gui_elements/line_edit/C_OgeLePropertiesName.cpp \
    ../src/opensyde_gui_elements/text_edit/C_OgeTedPropertiesComment.cpp \
    ../src/opensyde_gui_elements/text_edit/C_OgeTedToolTipBase.cpp \
    ../src/opensyde_gui_elements/text_edit/C_OgeTedContextMenuBase.cpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubConfigure.cpp \
    ../src/can_monitor/can_monitor_gui_elements/line_edit/C_CamOgeLeIdPopUp.cpp \
    ../src/opensyde_gui_elements/check_box/C_OgeChxProperties.cpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxText.cpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxIconOnly.cpp \
    ../src/can_monitor/project/C_CamProFilterData.cpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubIconOnly.cpp \
    ../src/opensyde_gui_elements/scroll_area/C_OgeSaMain.cpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosFilterTableView.cpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosFilterTableModel.cpp \
    ../src/can_monitor/message_generator/signals/C_CamGenSigTableView.cpp \
    ../src/can_monitor/message_generator/signals/C_CamGenSigTableModel.cpp \
    ../src/can_monitor/message_generator/signals/C_CamGenSigWidget.cpp \
    ../src/table_base/C_TblViewInteraction.cpp \
    ../src/can_monitor/message_trace/C_CamMetTreeLoggerData.cpp \
    ../src/can_monitor/can_monitor_gui_elements/group_box/C_CamOgeGbxMessageSignalsBorder.cpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosDatabaseItemWidget.cpp \
    ../src/can_monitor/project/C_CamProDatabaseData.cpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosDatabaseBusSelectionPopup.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabHeadingGroupBold.cpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosSectionPopup.cpp \
    ../src/can_monitor/message_generator/signals/C_CamGenSigTableDelegate.cpp \
    ../src/can_monitor/message_generator/C_CamGenKeySelect.cpp \
    ../src/can_monitor/can_monitor_gui_elements/spin_box/C_CamOgeSpxProperties.cpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxAllBase.cpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxToolTipBase.cpp \
    ../src/opensyde_gui_elements/widget/C_OgeWiSpinBoxGroup.cpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxAutoFixBase.cpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxDoubleAutoFix.cpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxInt64.cpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxInt64AutoFix.cpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxDoubleToolTipBase.cpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxInt64ToolTipBase.cpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxDoubleDynamicDecimalsBase.cpp \
    ../src/can_monitor/can_monitor_gui_elements/widget/C_CamOgeWiSpinBoxGroupTable.cpp \
    ../src/can_monitor/can_monitor_gui_elements/combo_box/C_CamOgeCbxDark.cpp \
    ../src/can_monitor/can_monitor_gui_elements/push_button/C_CamOgePubDarkBrowse.cpp \
    ../src/can_monitor/project/C_CamProLoggingData.cpp \
    ../src/can_monitor/data_base/C_CamDbDbc.cpp \
    ../src/can_monitor/data_base/C_CamDbDbcMessageId.cpp \
    ../src/can_monitor/data_base/C_CamDbHandler.cpp \
    ../src/can_monitor/data_base/C_CamDbOsy.cpp \
    ../src/can_monitor/message_generator/signals/C_CamGenSigUtil.cpp \
    ../src/can_monitor/can_monitor_gui_elements/menu/C_CamOgeMuRecentProjects.cpp \
    ../src/opensyde_gui_elements/line_edit/C_OgeLeProperties.cpp \
    ../src/table_base/tree_base/C_TblTreDataElementSortFilter.cpp \
    ../src/opensyde_gui_elements/C_OgeTreeViewToolTipBase.cpp \
    ../src/system_definition/C_SdTooltipUtil.cpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosDatabaseSelectionModel.cpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosDatabaseSelectionPopup.cpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosDatabaseSelectionView.cpp \
    ../src/can_monitor/can_monitor_gui_elements/combo_box/C_CamOgeCbxTableSmall.cpp \
    ../src/can_monitor/can_monitor_gui_elements/tool_button/C_CamOgeTobRecentProjects.cpp \
    ../src/opensyde_gui_elements/tool_button/C_OgeTobToolTipBase.cpp \
    ../src/can_monitor/util/C_CamUti.cpp \
    ../src/can_monitor/message_generator/C_CamGenTableDelegate.cpp \
    ../src/table_base/C_TblDelegateUtil.cpp \
    ../src/opensyde_gui_elements/C_OgeTreeToolTipBase.cpp \
    ../src/can_monitor/can_monitor_gui_elements/push_button/C_CamOgePubPathVariables.cpp \
    ../src/opensyde_gui_elements/line_edit/C_OgeLeFilePathBase.cpp \
    ../src/can_monitor/can_monitor_gui_elements/line_edit/C_CamOgeLeFilePath.cpp \
    ../src/opensyde_gui_elements/menu/C_OgeMuSections.cpp \
    ../src/table_base/tree_base/C_TblTreSimpleItem.cpp \
    ../src/table_base/tree_base/C_TblTreSimpleModel.cpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxTableBase.cpp \
    ../src/can_monitor/message_trace/C_CamMetTreeLoggerDataGreyOutInformation.cpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubMessageCancel.cpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubMessageOk.cpp \
    ../src/can_monitor/table_base/C_CamTblDelegate.cpp \
    ../src/can_monitor/message_trace/C_CamMetUtil.cpp \
    ../src/can_monitor/message_trace/C_CamMetSettingsPopup.cpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxMultiSelect.cpp \
    ../src/opensyde_gui_elements/label/C_OgeLabElided.cpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxDoubleAutoFixCustomTrigger.cpp \
    ../src/can_monitor/data_base/C_CamDbOsyMessageId.cpp \
    ../src/can_monitor/data_base/C_CamDbOsyListId.cpp \
    ../src/can_monitor/data_base/C_CamDbDbcUnmappedMessageId.cpp

PRECOMPILED_HEADER = ../src/precompiled_headers/can_monitor/precomp_headers.hpp

HEADERS  += \
    ../src/gettext/C_GtGetText.hpp \
    ../src/util/C_Uti.hpp \
    ../src/system_views/communication/C_SyvComMessageMonitor.hpp \
    ../src/system_views/communication/C_SyvComMessageLoggerFileBlf.hpp \
    ../src/system_views/communication/C_SyvComDriverThread.hpp \
    ../libs/dbc_driver_library/src/Vector/DBC/Attribute.h \
    ../libs/dbc_driver_library/src/Vector/DBC/AttributeDefinition.h \
    ../libs/dbc_driver_library/src/Vector/DBC/AttributeRelation.h \
    ../libs/dbc_driver_library/src/Vector/DBC/AttributeValueType.h \
    ../libs/dbc_driver_library/src/Vector/DBC/BitTiming.h \
    ../libs/dbc_driver_library/src/Vector/DBC/ByteOrder.h \
    ../libs/dbc_driver_library/src/Vector/DBC/config.h \
    ../libs/dbc_driver_library/src/Vector/DBC/Doxygen.h \
    ../libs/dbc_driver_library/src/Vector/DBC/EnvironmentVariable.h \
    ../libs/dbc_driver_library/src/Vector/DBC/ExtendedMultiplexor.h \
    ../libs/dbc_driver_library/src/Vector/DBC/File.h \
    ../libs/dbc_driver_library/src/Vector/DBC/Message.h \
    ../libs/dbc_driver_library/src/Vector/DBC/Network.h \
    ../libs/dbc_driver_library/src/Vector/DBC/Node.h \
    ../libs/dbc_driver_library/src/Vector/DBC/platform.h \
    ../libs/dbc_driver_library/src/Vector/DBC/Signal.h \
    ../libs/dbc_driver_library/src/Vector/DBC/SignalGroup.h \
    ../libs/dbc_driver_library/src/Vector/DBC/SignalType.h \
    ../libs/dbc_driver_library/src/Vector/DBC/Status.h \
    ../libs/dbc_driver_library/src/Vector/DBC/ValueDescriptions.h \
    ../libs/dbc_driver_library/src/Vector/DBC/ValueTable.h \
    ../libs/dbc_driver_library/src/Vector/DBC/ValueType.h \
    ../libs/dbc_driver_library/src/Vector/DBC/vector_dbc_export.h \
    ../libs/dbc_driver_library/src/Vector/DBC.h \
    ../src/com_import_export/C_CieImportDbc.hpp \
    ../src/com_import_export/C_CieConverter.hpp \
    ../src/system_definition/node_edit/datapools/C_SdNdeDpContentUtil.hpp \
    ../src/project_operations/C_PopErrorHandling.hpp \
    ../src/opensyde_gui_elements/widget/C_OgeWiCustomMessage.hpp \
    ../src/opensyde_gui_elements/widget/C_OgeWiError.hpp \
    ../src/opensyde_gui_elements/C_OgePopUpDialog.hpp \
    ../src/opensyde_gui_elements/C_OgeOverlay.hpp \
    ../src/opensyde_gui_elements/group_box/C_OgeGbxMouseMove.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabHeadingMessage.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabDescriptionMessage.hpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubCancel.hpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubDialog.hpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubIconText.hpp \
    ../src/opensyde_gui_elements/text_browser/C_OgeTebMessageDetails.hpp \
    ../src/navigable_gui/C_NagToolTip.hpp \
    ../src/opensyde_gui_elements/widget/C_OgeWiUtil.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabPopUpTitle.hpp \
    ../src/help_engine/C_HeHandler.hpp \
    ../src/opensyde_gui_elements/group_box/C_OgeGbxHover.hpp \
    ../src/opensyde_gui_elements/group_box/C_OgeGbxTransparent.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabContextMenuBase.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabPopUpSubTitle.hpp \
    ../src/opensyde_gui_elements/frame/C_OgeFraBorder.hpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubStandard.hpp \
    ../src/opensyde_gui_elements/text_browser/C_OgeTebContextMenuBase.hpp \
    ../src/opensyde_gui_elements/group_box/C_OgeGbxToolTip.hpp \
    ../src/opensyde_gui_elements/group_box/C_OgeGbxToolTipBase.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabHeadingToolTip.hpp \
    ../src/opensyde_gui_elements/text_edit/C_OgeTedToolTipContent.hpp \
    ../src/opensyde_gui_elements/C_OgeContextMenu.hpp \
    ../src/opensyde_gui_elements/C_OgeToolTipBase.hpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubToolTipBase.hpp \
    ../src/can_monitor/C_CamMainWindow.hpp \
    ../src/opensyde_gui_elements/widget/C_OgeWiOnlyBackground.hpp \
    ../src/opensyde_gui_elements/widget/C_OgeWiWithToolTip.hpp \
    ../src/can_monitor/message_trace/C_CamMetTreeDelegate.hpp \
    ../src/can_monitor/message_trace/C_CamMetTreeGuiBuffer.hpp \
    ../src/can_monitor/message_trace/C_CamMetTreeModel.hpp \
    ../src/can_monitor/message_trace/C_CamMetTreeView.hpp \
    ../src/table_base/tree_base/C_TblTreItem.hpp \
    ../src/table_base/tree_base/C_TblTreModel.hpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubSvgIconOnly.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabSvgOnly.hpp \
    ../src/can_monitor/cam_constants.hpp \
    ../src/can_monitor/user_settings/C_UsHandler.hpp \
    ../src/can_monitor/user_settings/C_UsFiler.hpp \
    ../src/navigable_gui/C_NagAboutDialog.hpp \
    ../src/opensyde_gui_elements/group_box/C_OgeGbx.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabGroupItem.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabGroupItemValue.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabToolTipBase.hpp \
    ../src/can_monitor/C_CamComDriverBase.hpp \
    ../src/can_monitor/message_trace/C_CamMetWidget.hpp \
    ../src/can_monitor/message_trace/C_CamMetStatusBarWidget.hpp \
    ../src/can_monitor/message_trace/C_CamMetControlBarWidget.hpp \
    ../src/can_monitor/message_generator/C_CamGenWidget.hpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosWidget.hpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosDllWidget.hpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosFilterWidget.hpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosDatabaseWidget.hpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosLoggingWidget.hpp \
    ../src/can_monitor/C_CamTitleBarWidget.hpp \
    ../src/can_monitor/util/C_CamUtiStyleSheets.hpp \
    ../src/can_monitor/can_monitor_gui_elements/label/C_CamOgeLabStatusBar.hpp \
    ../src/constants.hpp \
    ../src/util/C_UtiClipBoardHelper.hpp \
    ../src/can_monitor/message_trace/C_CamMetClipBoardHelper.hpp \
    ../src/can_monitor/can_monitor_gui_elements/splitter/C_CamOgeSpi.hpp \
    ../src/opensyde_gui_elements/splitter/C_OgeSpiBase.hpp \
    ../src/can_monitor/can_monitor_gui_elements/widget/C_CamOgeWiSectionHeader.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabGenericNoPaddingNoMargins.hpp \
    ../src/can_monitor/can_monitor_gui_elements/widget/C_CamOgeWiSettingSubSection.hpp \
    ../src/opensyde_gui_elements/radio_button/C_OgeRabProperties.hpp \
    ../src/opensyde_gui_elements/radio_button/C_OgeRabToolTipBase.hpp \
    ../src/opensyde_gui_elements/frame/C_OgeFraSeparator.hpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubProjAction.hpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxToolTipBase.hpp \
    ../src/can_monitor/can_monitor_gui_elements/combo_box/C_CamOgeCbxWhite.hpp \
    ../src/can_monitor/can_monitor_gui_elements/push_button/C_CamOgePubAbout.hpp \
    ../src/can_monitor/message_generator/C_CamGenTableView.hpp \
    ../src/can_monitor/message_generator/C_CamGenTableModel.hpp \
    ../src/opensyde_gui_elements/line_edit/C_OgeLeContextMenuBase.hpp \
    ../src/opensyde_gui_elements/line_edit/C_OgeLeDark.hpp \
    ../src/util/C_UtiFindNameHelper.hpp \
    ../src/can_monitor/message_generator/C_CamGenMessagesWidget.hpp \
    ../src/can_monitor/project/C_CamProHandler.hpp \
    ../src/can_monitor/project/C_CamProHandlerFiler.hpp \
    ../src/can_monitor/project/C_CamProMessageData.hpp \
    ../src/can_monitor/can_monitor_gui_elements/push_button/C_CamOgePubProjOp.hpp \
    ../src/can_monitor/project/C_CamProClipBoardHelper.hpp \
    ../src/table_base/C_TblModelAction.hpp \
    ../src/can_monitor/can_monitor_gui_elements/push_button/C_CamOgePubSettingsAdd.hpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosFilterItemWidget.hpp \
    ../src/opensyde_gui_elements/C_OgeTransparentScrollArea.hpp \
    ../src/can_monitor/can_monitor_gui_elements/check_box/C_CamOgeChxSettings.hpp \
    ../src/opensyde_gui_elements/check_box/C_OgeChxToolTipBase.hpp \
    ../src/table_base/C_TblViewScroll.hpp \
    ../src/table_base/C_TblViewToolTipBase.hpp \
    ../src/table_base/C_TblDelegate.hpp \
    ../src/opensyde_gui_elements/line_edit/C_OgeLeToolTipBase.hpp \
    ../src/can_monitor/can_monitor_gui_elements/combo_box/C_CamOgeCbxTable.hpp \
    ../src/can_monitor/can_monitor_gui_elements/line_edit/C_CamOgeLeTableEdit.hpp \
    ../src/table_base/C_TblEditLineEditBase.hpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxIconDelegate.hpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxResizingView.hpp \
    ../src/can_monitor/can_monitor_gui_elements/group_box/C_CamOgeGbxMessageGeneratorBackground.hpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosFilterPopup.hpp \
    ../src/opensyde_gui_elements/line_edit/C_OgeLePropertiesName.hpp \
    ../src/opensyde_gui_elements/text_edit/C_OgeTedPropertiesComment.hpp \
    ../src/opensyde_gui_elements/text_edit/C_OgeTedToolTipBase.hpp \
    ../src/opensyde_gui_elements/text_edit/C_OgeTedContextMenuBase.hpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubConfigure.hpp \
    ../src/can_monitor/can_monitor_gui_elements/line_edit/C_CamOgeLeIdPopUp.hpp \
    ../src/opensyde_gui_elements/check_box/C_OgeChxProperties.hpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxText.hpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxIconOnly.hpp \
    ../src/can_monitor/project/C_CamProFilterData.hpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubIconOnly.hpp \
    ../src/opensyde_gui_elements/scroll_area/C_OgeSaMain.hpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosFilterTableView.hpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosFilterTableModel.hpp \
    ../src/can_monitor/message_generator/signals/C_CamGenSigTableView.hpp \
    ../src/can_monitor/message_generator/signals/C_CamGenSigTableModel.hpp \
    ../src/can_monitor/message_generator/signals/C_CamGenSigWidget.hpp \
    ../src/table_base/C_TblViewInteraction.hpp \
    ../src/can_monitor/message_trace/C_CamMetTreeLoggerData.hpp \
    ../src/can_monitor/can_monitor_gui_elements/group_box/C_CamOgeGbxMessageSignalsBorder.hpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosDatabaseItemWidget.hpp \
    ../src/can_monitor/project/C_CamProDatabaseData.hpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosDatabaseBusSelectionPopup.hpp \
    ../libs/blf_driver_library/BLF.h \
    ../libs/blf_driver_library/BLF/AbstractFile.h \
    ../libs/blf_driver_library/BLF/AfdxFrame.h \
    ../libs/blf_driver_library/BLF/AfdxStatistic.h \
    ../libs/blf_driver_library/BLF/AppText.h \
    ../libs/blf_driver_library/BLF/AppTrigger.h \
    ../libs/blf_driver_library/BLF/CanDriverError.h \
    ../libs/blf_driver_library/BLF/CanDriverErrorExt.h \
    ../libs/blf_driver_library/BLF/CanDriverHwSync.h \
    ../libs/blf_driver_library/BLF/CanDriverStatistic.h \
    ../libs/blf_driver_library/BLF/CanErrorFrame.h \
    ../libs/blf_driver_library/BLF/CanErrorFrameExt.h \
    ../libs/blf_driver_library/BLF/CanFdErrorFrame64.h \
    ../libs/blf_driver_library/BLF/CanFdMessage.h \
    ../libs/blf_driver_library/BLF/CanFdMessage64.h \
    ../libs/blf_driver_library/BLF/CanMessage.h \
    ../libs/blf_driver_library/BLF/CanMessage2.h \
    ../libs/blf_driver_library/BLF/CanOverloadFrame.h \
    ../libs/blf_driver_library/BLF/CompressedFile.h \
    ../libs/blf_driver_library/BLF/config.h \
    ../libs/blf_driver_library/BLF/Doxygen.h \
    ../libs/blf_driver_library/BLF/DriverOverrun.h \
    ../libs/blf_driver_library/BLF/EnvironmentVariable.h \
    ../libs/blf_driver_library/BLF/EthernetFrame.h \
    ../libs/blf_driver_library/BLF/EthernetRxError.h \
    ../libs/blf_driver_library/BLF/EthernetStatus.h \
    ../libs/blf_driver_library/BLF/EventComment.h \
    ../libs/blf_driver_library/BLF/Exceptions.h \
    ../libs/blf_driver_library/BLF/File.h \
    ../libs/blf_driver_library/BLF/FileStatistics.h \
    ../libs/blf_driver_library/BLF/FlexRayData.h \
    ../libs/blf_driver_library/BLF/FlexRayStatusEvent.h \
    ../libs/blf_driver_library/BLF/FlexRaySync.h \
    ../libs/blf_driver_library/BLF/FlexRayV6Message.h \
    ../libs/blf_driver_library/BLF/FlexRayV6StartCycleEvent.h \
    ../libs/blf_driver_library/BLF/FlexRayVFrError.h \
    ../libs/blf_driver_library/BLF/FlexRayVFrReceiveMsg.h \
    ../libs/blf_driver_library/BLF/FlexRayVFrReceiveMsgEx.h \
    ../libs/blf_driver_library/BLF/FlexRayVFrStartCycle.h \
    ../libs/blf_driver_library/BLF/FlexRayVFrStatus.h \
    ../libs/blf_driver_library/BLF/GlobalMarker.h \
    ../libs/blf_driver_library/BLF/GpsEvent.h \
    ../libs/blf_driver_library/BLF/J1708Message.h \
    ../libs/blf_driver_library/BLF/KLineStatusEvent.h \
    ../libs/blf_driver_library/BLF/LinBaudrateEvent.h \
    ../libs/blf_driver_library/BLF/LinBusEvent.h \
    ../libs/blf_driver_library/BLF/LinChecksumInfo.h \
    ../libs/blf_driver_library/BLF/LinCrcError.h \
    ../libs/blf_driver_library/BLF/LinCrcError2.h \
    ../libs/blf_driver_library/BLF/LinDatabyteTimestampEvent.h \
    ../libs/blf_driver_library/BLF/LinDisturbanceEvent.h \
    ../libs/blf_driver_library/BLF/LinDlcInfo.h \
    ../libs/blf_driver_library/BLF/LinLongDomSignalEvent.h \
    ../libs/blf_driver_library/BLF/LinLongDomSignalEvent2.h \
    ../libs/blf_driver_library/BLF/LinMessage.h \
    ../libs/blf_driver_library/BLF/LinMessage2.h \
    ../libs/blf_driver_library/BLF/LinMessageDescriptor.h \
    ../libs/blf_driver_library/BLF/LinReceiveError.h \
    ../libs/blf_driver_library/BLF/LinReceiveError2.h \
    ../libs/blf_driver_library/BLF/LinSchedulerModeChange.h \
    ../libs/blf_driver_library/BLF/LinSendError.h \
    ../libs/blf_driver_library/BLF/LinSendError2.h \
    ../libs/blf_driver_library/BLF/LinShortOrSlowResponse.h \
    ../libs/blf_driver_library/BLF/LinSlaveTimeout.h \
    ../libs/blf_driver_library/BLF/LinSleepModeEvent.h \
    ../libs/blf_driver_library/BLF/LinSpikeEvent.h \
    ../libs/blf_driver_library/BLF/LinSpikeEvent2.h \
    ../libs/blf_driver_library/BLF/LinStatisticEvent.h \
    ../libs/blf_driver_library/BLF/LinSyncError.h \
    ../libs/blf_driver_library/BLF/LinSyncError2.h \
    ../libs/blf_driver_library/BLF/LinSynchFieldEvent.h \
    ../libs/blf_driver_library/BLF/LinUnexpectedWakeup.h \
    ../libs/blf_driver_library/BLF/LinWakeupEvent.h \
    ../libs/blf_driver_library/BLF/LinWakeupEvent2.h \
    ../libs/blf_driver_library/BLF/LogContainer.h \
    ../libs/blf_driver_library/BLF/Most50Message.h \
    ../libs/blf_driver_library/BLF/Most50Pkt.h \
    ../libs/blf_driver_library/BLF/Most150AllocTab.h \
    ../libs/blf_driver_library/BLF/Most150Message.h \
    ../libs/blf_driver_library/BLF/Most150MessageFragment.h \
    ../libs/blf_driver_library/BLF/Most150Pkt.h \
    ../libs/blf_driver_library/BLF/Most150PktFragment.h \
    ../libs/blf_driver_library/BLF/MostAllocTab.h \
    ../libs/blf_driver_library/BLF/MostCtrl.h \
    ../libs/blf_driver_library/BLF/MostDataLost.h \
    ../libs/blf_driver_library/BLF/MostEcl.h \
    ../libs/blf_driver_library/BLF/MostEthernetPkt.h \
    ../libs/blf_driver_library/BLF/MostEthernetPktFragment.h \
    ../libs/blf_driver_library/BLF/MostGenReg.h \
    ../libs/blf_driver_library/BLF/MostHwMode.h \
    ../libs/blf_driver_library/BLF/MostLightLock.h \
    ../libs/blf_driver_library/BLF/MostNetState.h \
    ../libs/blf_driver_library/BLF/MostPkt.h \
    ../libs/blf_driver_library/BLF/MostPkt2.h \
    ../libs/blf_driver_library/BLF/MostReg.h \
    ../libs/blf_driver_library/BLF/MostSpy.h \
    ../libs/blf_driver_library/BLF/MostStatistic.h \
    ../libs/blf_driver_library/BLF/MostStatisticEx.h \
    ../libs/blf_driver_library/BLF/MostStress.h \
    ../libs/blf_driver_library/BLF/MostSystemEvent.h \
    ../libs/blf_driver_library/BLF/MostTrigger.h \
    ../libs/blf_driver_library/BLF/MostTxLight.h \
    ../libs/blf_driver_library/BLF/ObjectHeader.h \
    ../libs/blf_driver_library/BLF/ObjectHeader2.h \
    ../libs/blf_driver_library/BLF/ObjectHeaderBase.h \
    ../libs/blf_driver_library/BLF/platform.h \
    ../libs/blf_driver_library/BLF/RealtimeClock.h \
    ../libs/blf_driver_library/BLF/SerialEvent.h \
    ../libs/blf_driver_library/BLF/SystemVariable.h \
    ../libs/blf_driver_library/BLF/UncompressedFile.h \
    ../libs/blf_driver_library/BLF/vector_blf_export.h \
    ../libs/blf_driver_library/BLF/VectorTypes.h \
    ../libs/blf_driver_library/BLF/WlanFrame.h \
    ../libs/blf_driver_library/BLF/WlanStatistic.h \
    ../src/opensyde_gui_elements/label/C_OgeLabHeadingGroupBold.hpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosSectionPopup.hpp \
    ../src/can_monitor/message_generator/signals/C_CamGenSigTableDelegate.hpp \
    ../src/can_monitor/message_generator/C_CamGenKeySelect.hpp \
    ../src/can_monitor/can_monitor_gui_elements/spin_box/C_CamOgeSpxProperties.hpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxAllBase.hpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxToolTipBase.hpp \
    ../src/opensyde_gui_elements/widget/C_OgeWiSpinBoxGroup.hpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxAutoFixBase.hpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxDoubleAutoFix.hpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxInt64.hpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxInt64AutoFix.hpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxDoubleToolTipBase.hpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxInt64ToolTipBase.hpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxDoubleDynamicDecimalsBase.hpp \
    ../src/can_monitor/can_monitor_gui_elements/widget/C_CamOgeWiSpinBoxGroupTable.hpp \
    ../src/can_monitor/can_monitor_gui_elements/combo_box/C_CamOgeCbxDark.hpp \
    ../src/can_monitor/can_monitor_gui_elements/push_button/C_CamOgePubDarkBrowse.hpp \
    ../src/can_monitor/project/C_CamProLoggingData.hpp \
    ../src/can_monitor/data_base/C_CamDbDbc.hpp \
    ../src/can_monitor/data_base/C_CamDbDbcMessageId.hpp \
    ../src/can_monitor/data_base/C_CamDbHandler.hpp \
    ../src/can_monitor/data_base/C_CamDbOsy.hpp \
    ../src/can_monitor/message_generator/signals/C_CamGenSigUtil.hpp \
    ../src/can_monitor/can_monitor_gui_elements/menu/C_CamOgeMuRecentProjects.hpp \
    ../src/opensyde_gui_elements/line_edit/C_OgeLeProperties.hpp \
    ../src/table_base/tree_base/C_TblTreDataElementSortFilter.hpp \
    ../src/opensyde_gui_elements/C_OgeTreeViewToolTipBase.hpp \
    ../src/system_definition/C_SdTooltipUtil.hpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosDatabaseSelectionModel.hpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosDatabaseSelectionPopup.hpp \
    ../src/can_monitor/can_monitor_settings/C_CamMosDatabaseSelectionView.hpp \
    ../src/can_monitor/can_monitor_gui_elements/combo_box/C_CamOgeCbxTableSmall.hpp \
    ../src/can_monitor/can_monitor_gui_elements/tool_button/C_CamOgeTobRecentProjects.hpp \
    ../src/opensyde_gui_elements/tool_button/C_OgeTobToolTipBase.hpp \
    ../src/can_monitor/util/C_CamUti.hpp \
    ../src/can_monitor/message_generator/C_CamGenTableDelegate.hpp \
    ../src/table_base/C_TblDelegateUtil.hpp \
    ../src/opensyde_gui_elements/C_OgeTreeToolTipBase.hpp \
    ../src/can_monitor/can_monitor_gui_elements/push_button/C_CamOgePubPathVariables.hpp \
    ../src/opensyde_gui_elements/line_edit/C_OgeLeFilePathBase.hpp \
    ../src/can_monitor/can_monitor_gui_elements/line_edit/C_CamOgeLeFilePath.hpp \
    ../src/opensyde_gui_elements/menu/C_OgeMuSections.hpp \
    ../src/table_base/tree_base/C_TblTreSimpleItem.hpp \
    ../src/table_base/tree_base/C_TblTreSimpleModel.hpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxTableBase.hpp \
    ../src/can_monitor/message_trace/C_CamMetTreeLoggerDataGreyOutInformation.hpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubMessageCancel.hpp \
    ../src/opensyde_gui_elements/push_button/C_OgePubMessageOk.hpp \
    ../src/can_monitor/table_base/C_CamTblDelegate.hpp \
    ../src/can_monitor/message_trace/C_CamMetUtil.hpp \
    ../src/can_monitor/message_trace/C_CamMetSettingsPopup.hpp \
    ../src/opensyde_gui_elements/combo_box/C_OgeCbxMultiSelect.hpp \
    ../src/opensyde_gui_elements/label/C_OgeLabElided.hpp \
    ../src/opensyde_gui_elements/spin_box/C_OgeSpxDoubleAutoFixCustomTrigger.hpp \
    ../src/can_monitor/data_base/C_CamDbOsyMessageId.hpp \
    ../src/can_monitor/data_base/C_CamDbOsyListId.hpp \
    ../src/can_monitor/data_base/C_CamDbDbcUnmappedMessageId.hpp

FORMS    += \
    ../src/opensyde_gui_elements/widget/C_OgeWiCustomMessage.ui \
    ../src/opensyde_gui_elements/widget/C_OgeWiError.ui \
    ../src/opensyde_gui_elements/C_OgePopUpDialog.ui \
    ../src/navigable_gui/C_NagToolTip.ui \
    ../src/can_monitor/C_CamMainWindow.ui \
    ../src/navigable_gui/C_NagAboutDialog.ui \
    ../src/can_monitor/message_trace/C_CamMetWidget.ui \
    ../src/can_monitor/message_trace/C_CamMetStatusBarWidget.ui \
    ../src/can_monitor/message_trace/C_CamMetControlBarWidget.ui \
    ../src/can_monitor/message_generator/C_CamGenWidget.ui \
    ../src/can_monitor/can_monitor_settings/C_CamMosWidget.ui \
    ../src/can_monitor/can_monitor_settings/C_CamMosDllWidget.ui \
    ../src/can_monitor/can_monitor_settings/C_CamMosFilterWidget.ui \
    ../src/can_monitor/can_monitor_settings/C_CamMosDatabaseWidget.ui \
    ../src/can_monitor/can_monitor_settings/C_CamMosLoggingWidget.ui \
    ../src/can_monitor/C_CamTitleBarWidget.ui \
    ../src/can_monitor/can_monitor_gui_elements/widget/C_CamOgeWiSectionHeader.ui \
    ../src/can_monitor/can_monitor_gui_elements/widget/C_CamOgeWiSettingSubSection.ui \
    ../src/can_monitor/message_generator/C_CamGenMessagesWidget.ui \
    ../src/can_monitor/can_monitor_settings/C_CamMosFilterItemWidget.ui \
    ../src/can_monitor/can_monitor_settings/C_CamMosFilterPopup.ui \
    ../src/can_monitor/message_generator/signals/C_CamGenSigWidget.ui \
    ../src/can_monitor/can_monitor_settings/C_CamMosDatabaseItemWidget.ui \
    ../src/can_monitor/can_monitor_settings/C_CamMosDatabaseBusSelectionPopup.ui \
    ../src/can_monitor/message_generator/C_CamGenKeySelect.ui \
    ../src/opensyde_gui_elements/widget/C_OgeWiSpinBoxGroup.ui \
    ../src/can_monitor/can_monitor_settings/C_CamMosDatabaseSelectionPopup.ui \
    ../src/can_monitor/message_trace/C_CamMetSettingsPopup.ui

INCLUDEPATH += ../src \
               ../src/can_monitor \
               ../src/can_monitor/data_base \
               ../src/can_monitor/project \
               ../src/can_monitor/user_settings \
               ../src/can_monitor/message_trace \
               ../src/can_monitor/can_monitor_settings \
               ../src/can_monitor/message_generator \
               ../src/can_monitor/message_generator/signals \
               ../src/can_monitor/can_monitor_gui_elements \
               ../src/can_monitor/can_monitor_gui_elements/check_box \
               ../src/can_monitor/can_monitor_gui_elements/combo_box \
               ../src/can_monitor/can_monitor_gui_elements/group_box \
               ../src/can_monitor/can_monitor_gui_elements/push_button \
               ../src/can_monitor/can_monitor_gui_elements/label \
               ../src/can_monitor/can_monitor_gui_elements/line_edit \
               ../src/can_monitor/can_monitor_gui_elements/menu \
               ../src/can_monitor/can_monitor_gui_elements/splitter \
               ../src/can_monitor/can_monitor_gui_elements/spin_box \
               ../src/can_monitor/can_monitor_gui_elements/tool_button \
               ../src/can_monitor/can_monitor_gui_elements/widget \
               ../src/can_monitor/table_base \
               ../src/can_monitor/util \
               ../src/help_engine \
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
               ../src/opensyde_gui_elements/splitter \
               ../src/opensyde_gui_elements/spin_box \
               ../src/opensyde_gui_elements/scroll_area \
               ../src/opensyde_gui_elements/text_browser \
               ../src/opensyde_gui_elements/text_edit \
               ../src/opensyde_gui_elements/tool_button \
               ../src/opensyde_gui_elements/widget \
               ../src/navigable_gui \
               ../src/com_import_export \
               ../src/table_base \
               ../src/table_base/tree_base \
               ../src/system_definition \
               ../src/system_definition/node_edit/datapools \
               ../src/system_views/communication \
               ../src/project_operations/ \
               ../src/gettext \
               ../src/util \
               ../src/precompiled_headers/can_monitor \
               ../libs/gettext \
               ../libs/dbc_driver_library/src \
               ../libs/dbc_driver_library/src/Vector \
               ../libs/blf_driver_library

#do not issue deprecation warnings (tested code can contain deprecated functions which we do want to provide)
win32-g++ {
QMAKE_CXXFLAGS += -Wno-deprecated-declarations
}

LIBS += -L../libs/gettext -lintl \
        -L../libs/blf_driver_library -lvector_blf \
        -lz

LIBS += -lws2_32   #WinSock
LIBS += -lIphlpapi #IP helper API

#add windows API libraries
LIBS += -lversion

QMAKE_TARGET_COMPANY = STW
QMAKE_TARGET_PRODUCT = openSYDE CAN Monitor
#QMAKE_TARGET_DESCRIPTION = TODO
#See C coding rules v4.00r0
QMAKE_TARGET_COPYRIGHT = STW (c) 2018

DISTFILES +=

RESOURCES += \
    ../src/can_monitor/cam_application.qrc

#compiler with function intrumentation (for "Embedded Profiler")
#QMAKE_CXXFLAGS += -finstrument-functions
#workaround for https://bugreports.qt.io/browse/QTBUG-35884 (for "Embedded Profiler")
#QMAKE_CXXFLAGS += -finstrument-functions-exclude-function-list=load,store
#create map file
#QMAKE_LFLAGS += -Wl,-Map=qt_test.map
#libraries for "Embedded Profiler"
#LIBS += -L$$PWD/eprofiler-windows32-mingw-intel-1.2.3/EProfiler/windows32-mingw-intel/lib/ -lEProfilerStatic
#LIBS += -lpthread
