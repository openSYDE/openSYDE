//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for create/edit filter dialog. (implementation)

   Widget for create/edit filter dialog.

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "C_CamMosFilterPopup.h"
#include "ui_C_CamMosFilterPopup.h"

#include "stwerrors.h"
#include "C_GtGetText.h"
#include "C_OgeWiUtil.h"
#include "TGLUtils.h"
#include "C_OgeWiCustomMessage.h"
#include "C_CamProHandler.h"
#include "C_CamDbHandler.h"
#include "C_CamMosDatabaseSelectionPopup.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_core;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]     orc_FilterData    filter data (name, comment, ...)
   \param[in,out] orc_Parent        Reference to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMosFilterPopup::C_CamMosFilterPopup(const stw_opensyde_gui_logic::C_CamProFilterData & orc_FilterData,
                                         stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_CamMosFilterPopup),
   mrc_ParentDialog(orc_Parent),
   mpc_TableModel(new stw_opensyde_gui_logic::C_CamMosFilterTableModel),
   mc_UneditedName(orc_FilterData.c_Name)
{
   C_CamProFilterData c_FilterData = orc_FilterData;

   this->mpc_Ui->setupUi(this);

   // register the widget for showing
   this->mrc_ParentDialog.SetWidget(this);

   // set model for view (important!)
   this->mpc_Ui->pc_TableView->setModel(mpc_TableModel);

   // initialize static names
   this->m_InitStaticNames();

   // set ID minima
   this->mpc_Ui->pc_LeCanIdStart->SetMinFromVariant("0x0");
   this->mpc_Ui->pc_LeCanIdEnd->SetMinFromVariant("0x0");

   // initialize data for filters of type CAN ID
   for (sint32 s32_Pos = 0; s32_Pos < c_FilterData.c_FilterItems.size(); s32_Pos++)
   {
      if (c_FilterData.c_FilterItems[s32_Pos].e_Type == C_CamProFilterItemData::eID_SINGLE)
      {
         // force equality of start and end ID if type is "CAN ID"
         c_FilterData.c_FilterItems[s32_Pos].u32_EndId = c_FilterData.c_FilterItems[s32_Pos].u32_StartId;

         // search for messages in databases
         c_FilterData.c_FilterItems[s32_Pos].c_DatabaseMatch = C_CamMosFilterTableModel::h_SearchMessageInDatabases(
            c_FilterData.c_FilterItems[s32_Pos].u32_StartId);
      }
   }

   // initialize data: tell table model and adapt GUI
   this->mpc_TableModel->SetFilterItemsData(c_FilterData.c_FilterItems);
   this->m_InitFromData(c_FilterData); // call after mpc_TableModel->SetFilterItemsData (!)

   // initialize GUI
   this->m_InitStaticGUIElements();

   // connects
   connect(this->mpc_Ui->pc_PushButtonSave, &C_OgePubDialog::clicked, this, &C_CamMosFilterPopup::m_OnOk);
   connect(this->mpc_Ui->pc_PushButtonCancel, &C_OgePubCancel::clicked, this, &C_CamMosFilterPopup::m_OnCancel);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxType,
           static_cast<void (C_OgeCbxText::*)(sintn)>(&C_OgeCbxText::currentIndexChanged),
           this, &C_CamMosFilterPopup::m_OnTypeChanged);
   connect(this->mpc_Ui->pc_PubAddFromDatabase, &C_OgePubConfigure::clicked, this,
           &C_CamMosFilterPopup::m_OnAddFromDatabase);
   connect(this->mpc_Ui->pc_TableView, &C_CamMosFilterTableView::clicked, this, &C_CamMosFilterPopup::m_OnIndexClicked);
   connect(this->mpc_Ui->pc_TableView->selectionModel(), &QItemSelectionModel::currentRowChanged,
           this, &C_CamMosFilterPopup::m_OnRowChanged);
   connect(this->mpc_Ui->pc_TableView, &C_CamMosFilterTableView::SigDeleteKeyPressed,
           this, &C_CamMosFilterPopup::m_OnDeleteKeyPressed);
   connect(this->mpc_Ui->pc_RabPass, &C_OgeRabProperties::toggled, this, &C_CamMosFilterPopup::m_OnRadioButtonToggle);
   connect(this->mpc_Ui->pc_LeCanIdStart, &C_CamOgeLeIDPopUp::editingFinished, this,
           &C_CamMosFilterPopup::m_OnStartIdEdited);
   connect(this->mpc_Ui->pc_LeCanIdEnd, &C_CamOgeLeIDPopUp::editingFinished, this,
           &C_CamMosFilterPopup::m_OnEndIdEdited);
   connect(this->mpc_Ui->pc_CheckBoxExtended, &C_OgeChxProperties::toggled, this,
           &C_CamMosFilterPopup::m_OnExtendedToggled);
   connect(this->mpc_Ui->pc_PubAddItem, &C_OgePubSvgIconOnly::clicked, this, &C_CamMosFilterPopup::m_OnAddFilterItem);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamMosFilterPopup::~C_CamMosFilterPopup(void)
{
   delete this->mpc_Ui;
   delete this->mpc_TableModel;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get filter data.

   Attention: enabled flag must be set separately!

   \return
   filter data
*/
//----------------------------------------------------------------------------------------------------------------------
C_CamProFilterData C_CamMosFilterPopup::GetFilterData(void) const
{
   C_CamProFilterData c_Retval;

   c_Retval.c_Name = this->mpc_Ui->pc_LineEditName->text().trimmed(); // cut white space from both ends
   c_Retval.c_Comment = this->mpc_Ui->pc_TedComment->toPlainText();
   c_Retval.c_FilterItems = this->mpc_TableModel->GetFilterItemsData();
   // c_Retval.q_Enabled can not be edited in this dialog

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterPopup::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<sintn>(Qt::Key_Return)))
   {
      if (((opc_KeyEvent->modifiers().testFlag(Qt::ControlModifier) == true) &&
           (opc_KeyEvent->modifiers().testFlag(Qt::AltModifier) == false)) &&
          (opc_KeyEvent->modifiers().testFlag(Qt::ShiftModifier) == false))
      {
         this->m_OnOk();
      }
      else
      {
         q_CallOrg = false;
      }
   }
   if (q_CallOrg == true)
   {
      QWidget::keyPressEvent(opc_KeyEvent);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterPopup::m_InitStaticNames(void) const
{
   this->mrc_ParentDialog.SetTitle(C_GtGetText::h_GetText("Filter"));
   this->mrc_ParentDialog.SetSubTitle(C_GtGetText::h_GetText("Configuration"));

   this->mpc_Ui->pc_LabelName->setText(C_GtGetText::h_GetText("Name"));
   this->mpc_Ui->pc_LabelComment->setText(C_GtGetText::h_GetText("Comment"));
   this->mpc_Ui->pc_TedComment->setPlaceholderText(C_GtGetText::h_GetText("Add your comment here ..."));

   this->mpc_Ui->pc_LabelFilterItems->setText(C_GtGetText::h_GetText("Filter Items"));
   this->mpc_Ui->pc_LabelNoteAnd->setText(
      C_GtGetText::h_GetText("Note: Filter items are linked with logical conjunction (\"and\")."));

   this->mpc_Ui->pc_LabelSettings->setText(C_GtGetText::h_GetText("Settings"));
   this->mpc_Ui->pc_LabNoSelection->setText(C_GtGetText::h_GetText(
                                               "No filter items declared, add any via the '+' button."));
   this->mpc_Ui->pc_LabType->setText(C_GtGetText::h_GetText("Type"));
   this->mpc_Ui->pc_LabMode->setText(C_GtGetText::h_GetText("Mode"));
   this->mpc_Ui->pc_LabCanIdStart->setText(C_GtGetText::h_GetText("ID"));
   this->mpc_Ui->pc_LabCanIdEnd->setText(C_GtGetText::h_GetText("To"));
   this->mpc_Ui->pc_RabPass->setText(C_GtGetText::h_GetText("Pass"));
   this->mpc_Ui->pc_RabStop->setText(C_GtGetText::h_GetText("Stop"));
   this->mpc_Ui->pc_CheckBoxExtended->setText(C_GtGetText::h_GetText("Extended ID"));
   this->mpc_Ui->pc_PubAddFromDatabase->setText(C_GtGetText::h_GetText("Add from Database"));

   this->mpc_Ui->pc_ComboBoxType->addItem(C_GtGetText::h_GetText("CAN ID Range"));
   this->mpc_Ui->pc_ComboBoxType->addItem(C_GtGetText::h_GetText("CAN ID"));

   // tool tips
   this->mpc_Ui->pc_LabelName->SetToolTipInformation(C_GtGetText::h_GetText("Name"),
                                                     C_GtGetText::h_GetText("Symbolic and unique filter name."));
   this->mpc_Ui->pc_LabelComment->SetToolTipInformation(C_GtGetText::h_GetText("Comment"),
                                                        C_GtGetText::h_GetText("Comment for this filter."));
   this->mpc_Ui->pc_PubAddItem->SetToolTipInformation("", C_GtGetText::h_GetText("Add new filter item."));
   this->mpc_Ui->pc_LabType->SetToolTipInformation(C_GtGetText::h_GetText("Type"),
                                                   C_GtGetText::h_GetText("Available types: CAN ID and CAN ID Range."));
   this->mpc_Ui->pc_LabMode->SetToolTipInformation(C_GtGetText::h_GetText("Mode"),
                                                   C_GtGetText::h_GetText("Pass or stop given CAN ID(s)"));
   this->mpc_Ui->pc_LabCanIdStart->SetToolTipInformation(C_GtGetText::h_GetText("Message ID"),
                                                         C_GtGetText::h_GetText("ID of filtered message."));
   this->mpc_Ui->pc_LabCanIdEnd->SetToolTipInformation(C_GtGetText::h_GetText("Message ID"),
                                                       C_GtGetText::h_GetText("ID of last filtered message."));
   this->mpc_Ui->pc_CheckBoxExtended->SetToolTipInformation(C_GtGetText::h_GetText("Extended"),
                                                            C_GtGetText::h_GetText(
                                                               "CAN Message ID is in extended format."));
   this->mpc_Ui->pc_PubAddFromDatabase->SetToolTipInformation(C_GtGetText::h_GetText("Add from database"),
                                                              C_GtGetText::h_GetText(
                                                                 "Add message from configured database."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize colors, fonts, ...
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterPopup::m_InitStaticGUIElements(void) const
{
   // label "Name"
   this->mpc_Ui->pc_LabelName->SetForegroundColor(4);
   this->mpc_Ui->pc_LabelName->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelName->SetFontPixel(14);

   // label "Comment"
   this->mpc_Ui->pc_LabelComment->SetForegroundColor(4);
   this->mpc_Ui->pc_LabelComment->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelComment->SetFontPixel(14);

   // label "Filter Items"
   this->mpc_Ui->pc_LabelFilterItems->SetForegroundColor(4);
   this->mpc_Ui->pc_LabelFilterItems->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelFilterItems->SetFontPixel(14);

   // button add filter item
   this->mpc_Ui->pc_PubAddItem->SetSvg("://images/IconAddEnabled.svg", "", "://images/IconAddEnabledHover.svg",
                                       "", "", "", "://images/IconAddEnabledPressed.svg");

   // label "Note: ..."
   this->mpc_Ui->pc_LabelNoteAnd->SetForegroundColor(8);
   this->mpc_Ui->pc_LabelNoteAnd->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelNoteAnd->SetFontPixel(11);

   // label "Settings"
   this->mpc_Ui->pc_LabelSettings->SetForegroundColor(4);
   this->mpc_Ui->pc_LabelSettings->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabelSettings->SetFontPixel(14);

   // label "No filter item selected ..."
   this->mpc_Ui->pc_LabNoSelection->SetForegroundColor(8);
   this->mpc_Ui->pc_LabNoSelection->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabNoSelection->SetFontPixel(12);

   // label "Type"
   this->mpc_Ui->pc_LabType->SetForegroundColor(8);
   this->mpc_Ui->pc_LabType->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabType->SetFontPixel(12);

   // label "Mode"
   this->mpc_Ui->pc_LabMode->SetForegroundColor(8);
   this->mpc_Ui->pc_LabMode->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabMode->SetFontPixel(12);

   // label "ID" or "From"
   this->mpc_Ui->pc_LabCanIdStart->SetForegroundColor(8);
   this->mpc_Ui->pc_LabCanIdStart->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabCanIdStart->SetFontPixel(12);

   // label "To"
   this->mpc_Ui->pc_LabCanIdEnd->SetForegroundColor(8);
   this->mpc_Ui->pc_LabCanIdEnd->SetBackgroundColor(-1);
   this->mpc_Ui->pc_LabCanIdEnd->SetFontPixel(12);

   // button "Add from Database"
   this->mpc_Ui->pc_PubAddFromDatabase->setIconSize(QSize(16, 16));
   this->mpc_Ui->pc_PubAddFromDatabase->setIcon(QIcon("://images/IconAddDataBase.svg"));

   // radio buttons
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_RabPass, "BrightBackground", true);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_RabStop, "BrightBackground", true);

   // table view (needs to be after setModel, i.e. in this case not in constructor of table view)
   this->mpc_Ui->pc_TableView->setColumnWidth(C_CamMosFilterTableModel::h_EnumToColumn(C_CamMosFilterTableModel::
                                                                                       eENABLED), 60);
   this->mpc_Ui->pc_TableView->setColumnWidth(C_CamMosFilterTableModel::h_EnumToColumn(C_CamMosFilterTableModel::
                                                                                       eTYPE), 90);
   this->mpc_Ui->pc_TableView->setColumnWidth(C_CamMosFilterTableModel::h_EnumToColumn(C_CamMosFilterTableModel::
                                                                                       eMODE), 60);
   this->mpc_Ui->pc_TableView->setColumnWidth(C_CamMosFilterTableModel::h_EnumToColumn(C_CamMosFilterTableModel::
                                                                                       eDETAILS), 288);
   this->mpc_Ui->pc_TableView->setColumnWidth(C_CamMosFilterTableModel::h_EnumToColumn(C_CamMosFilterTableModel::
                                                                                       eREMOVE), 60);
   //stretch last section
   this->mpc_Ui->pc_TableView->horizontalHeader()->setStretchLastSection(true);

   //set header resize mode (stretch)
   this->mpc_Ui->pc_TableView->horizontalHeader()->setSectionResizeMode(C_CamMosFilterTableModel::h_EnumToColumn(
                                                                           C_CamMosFilterTableModel::eENABLED),
                                                                        QHeaderView::Fixed);
   this->mpc_Ui->pc_TableView->horizontalHeader()->setSectionResizeMode(C_CamMosFilterTableModel::h_EnumToColumn(
                                                                           C_CamMosFilterTableModel::eTYPE),
                                                                        QHeaderView::Fixed);
   this->mpc_Ui->pc_TableView->horizontalHeader()->setSectionResizeMode(C_CamMosFilterTableModel::h_EnumToColumn(
                                                                           C_CamMosFilterTableModel::eMODE),
                                                                        QHeaderView::Fixed);
   this->mpc_Ui->pc_TableView->horizontalHeader()->setSectionResizeMode(C_CamMosFilterTableModel::h_EnumToColumn(
                                                                           C_CamMosFilterTableModel::eDETAILS),
                                                                        QHeaderView::Fixed);
   this->mpc_Ui->pc_TableView->horizontalHeader()->setSectionResizeMode(C_CamMosFilterTableModel::h_EnumToColumn(
                                                                           C_CamMosFilterTableModel::eREMOVE),
                                                                        QHeaderView::Fixed);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize data from filter data.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterPopup::m_InitFromData(const stw_opensyde_gui_logic::C_CamProFilterData & orc_FilterData) const
{
   const bool q_NoItems = orc_FilterData.c_FilterItems.isEmpty();

   // first section
   // name and comment
   this->mpc_Ui->pc_LineEditName->setText(orc_FilterData.c_Name);
   this->mpc_Ui->pc_TedComment->setText(orc_FilterData.c_Comment);

   // second section
   // show label "no filter item selected" or table and settings
   this->m_ShowNoFilter(q_NoItems);

   // select first row if possible
   if (q_NoItems == false)
   {
      this->mpc_Ui->pc_TableView->selectRow(0);
      // set type specific widgets to default "CAN ID"
      this->m_UpdateSettingsSection(orc_FilterData.c_FilterItems[0]);
   }

   // set filter items count
   this->m_UpdateTitleFilterItemCount();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update settings section (line edits, radio buttons, ...)

   \param[in]     orc_FilterData    Filter item data the settings correspond to
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterPopup::m_UpdateSettingsSection(const C_CamProFilterItemData & orc_FilterItemData) const
{
   // type
   if (orc_FilterItemData.e_Type == C_CamProFilterItemData::eID_SINGLE)
   {
      this->mpc_Ui->pc_ComboBoxType->setCurrentIndex(1);
   }
   else
   {
      this->mpc_Ui->pc_ComboBoxType->setCurrentIndex(0);
   }
   this->m_ShowTypeSpecificWidgets(this->mpc_Ui->pc_ComboBoxType->currentIndex()); // show type specific widgets

   // mode
   this->mpc_Ui->pc_RabPass->setChecked(orc_FilterItemData.q_PassFilter);
   this->mpc_Ui->pc_RabStop->setChecked(!orc_FilterItemData.q_PassFilter);

   // message IDs
   // second parameter of int type is important for line edit data handling
   this->mpc_Ui->pc_LeCanIdStart->SetFromVariant(mh_GetValueAsHex(orc_FilterItemData.u32_StartId),
                                                 static_cast<uint64>(orc_FilterItemData.u32_StartId));
   this->mpc_Ui->pc_LeCanIdEnd->SetFromVariant(mh_GetValueAsHex(orc_FilterItemData.u32_EndId),
                                               static_cast<uint64>(orc_FilterItemData.u32_EndId));

   // ID min/max (for tool tip in initial case)
   if (static_cast<bool>(orc_FilterItemData.u8_ExtendedId) == true)
   {
      this->mpc_Ui->pc_LeCanIdStart->SetMaxFromVariant("0x1FFFFFFF");
      this->mpc_Ui->pc_LeCanIdEnd->SetMaxFromVariant("0x1FFFFFFF");
   }
   else
   {
      this->mpc_Ui->pc_LeCanIdStart->SetMaxFromVariant("0x7FF");
      this->mpc_Ui->pc_LeCanIdEnd->SetMaxFromVariant("0x7FF");
   }

   // extended flag
   this->mpc_Ui->pc_CheckBoxExtended->setChecked(static_cast<bool>(orc_FilterItemData.u8_ExtendedId));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show label "No filter item" or table and settings.

   \param[in]     oq_NoFilter    true: show label "No filter item"; false: show settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterPopup::m_ShowNoFilter(const bool oq_NoFilter) const
{
   // these ifs are because order matters: first hide then show to avoid resizing
   if (oq_NoFilter == false)
   {
      this->mpc_Ui->pc_GroupBoxNoElements->setVisible(oq_NoFilter);
   }

   this->mpc_Ui->pc_WiSettings->setVisible(!oq_NoFilter);
   this->mpc_Ui->pc_TableView->setVisible(!oq_NoFilter);
   this->mpc_Ui->pc_LabelNoteAnd->setVisible(!oq_NoFilter);

   if (oq_NoFilter == true)
   {
      this->mpc_Ui->pc_GroupBoxNoElements->setVisible(oq_NoFilter);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show type specific widgets.

   \param[in]     os32_NewType     new index (corresponds to type)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterPopup::m_ShowTypeSpecificWidgets(const sint32 os32_NewType) const
{
   if (os32_NewType == 1)
   {
      // Type "ID"
      this->mpc_Ui->pc_LabCanIdStart->setText(C_GtGetText::h_GetText("ID"));
      this->mpc_Ui->pc_LabCanIdStart->SetToolTipInformation(C_GtGetText::h_GetText("Message ID"),
                                                            C_GtGetText::h_GetText("ID of filtered message."));
      this->mpc_Ui->pc_LabCanIdEnd->setVisible(false);
      this->mpc_Ui->pc_LeCanIdEnd->setVisible(false); // first hide then show to avoid resizing
      this->mpc_Ui->pc_PubAddFromDatabase->setVisible(true);
   }
   else
   {
      // Default: Type "ID Range"
      this->mpc_Ui->pc_LabCanIdStart->setText(C_GtGetText::h_GetText("From"));
      this->mpc_Ui->pc_LabCanIdStart->SetToolTipInformation(C_GtGetText::h_GetText("Message ID"),
                                                            C_GtGetText::h_GetText("ID of first filtered message."));
      this->mpc_Ui->pc_LabCanIdEnd->setVisible(true);
      this->mpc_Ui->pc_PubAddFromDatabase->setVisible(false); // first hide then show to avoid resizing
      this->mpc_Ui->pc_LeCanIdEnd->setVisible(true);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update both line edit texts with table data.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterPopup::m_UpdateLineEdits(const uint32 ou32_RowIndex) const
{
   const QList<C_CamProFilterItemData> c_Data = this->mpc_TableModel->GetFilterItemsData();

   // check if index is in range
   tgl_assert(c_Data.size() >= static_cast<sintn>(ou32_RowIndex));
   if (c_Data.size() >= static_cast<sintn>(ou32_RowIndex))
   {
      const C_CamProFilterItemData c_FilterItem = c_Data[ou32_RowIndex];

      this->mpc_Ui->pc_LeCanIdStart->SetFromVariant(mh_GetValueAsHex(c_FilterItem.u32_StartId),
                                                    static_cast<uint64>(c_FilterItem.u32_StartId));
      this->mpc_Ui->pc_LeCanIdEnd->SetFromVariant(mh_GetValueAsHex(c_FilterItem.u32_EndId),
                                                  static_cast<uint64>(c_FilterItem.u32_EndId));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterPopup::m_OnCancel(void) const
{
   this->mrc_ParentDialog.reject();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterPopup::m_OnOk(void)
{
   const QString c_Name = this->mpc_Ui->pc_LineEditName->text().trimmed();
   C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eERROR);
   bool q_Accept = true;

   c_Message.SetHeading(C_GtGetText::h_GetText("Filter Name"));

   // do not allow empty names
   if (c_Name.isEmpty() == true)
   {
      q_Accept = false;
      c_Message.SetDescription(C_GtGetText::h_GetText("Filter name is empty. Choose a valid name."));
   }
   else
   {
      // check if name is duplicate (only if edited to avoid comparison with itself)
      if (c_Name != this->mc_UneditedName)
      {
         if (C_CamProHandler::h_GetInstance()->CheckFilterNameAvailable(c_Name) == false)
         {
            q_Accept = false;
            c_Message.SetDescription(C_GtGetText::h_GetText("Filter name is already used. Choose another name."));
         }
      }
   }

   if (q_Accept == true)
   {
      this->mrc_ParentDialog.accept();
   }
   else
   {
      c_Message.Execute();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of add filter item button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterPopup::m_OnAddFilterItem(void) const
{
   std::vector<uint32> c_IndexVector;
   uint32 u32_NewIndex;

   // add new row in model
   c_IndexVector.push_back(static_cast<uint32>(this->mpc_TableModel->rowCount()));
   u32_NewIndex = this->mpc_TableModel->AddNewItem(c_IndexVector);

   // select new row
   this->mpc_Ui->pc_TableView->clearSelection();
   this->mpc_Ui->pc_TableView->selectRow(u32_NewIndex);
   this->mpc_Ui->pc_TableView->setFocus();

   // update filter items count
   this->m_UpdateTitleFilterItemCount();

   // show or hide table and settings
   this->m_ShowNoFilter(false);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of index/row click of table view.

   Here: detect remove click and delete if so
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterPopup::m_OnIndexClicked(const QModelIndex & orc_NewIndex) const
{
   sint32 s32_Row = orc_NewIndex.row();
   const sint32 s32_Column = orc_NewIndex.column();

   // distinguish remove clicked from rest
   if (s32_Column == C_CamMosFilterTableModel::h_EnumToColumn(C_CamMosFilterTableModel::eREMOVE))
   {
      std::vector<uint32> c_IndexToDelete;
      c_IndexToDelete.push_back(s32_Row);
      this->mpc_TableModel->DeleteSelectedItems(c_IndexToDelete);

      // update filter items count
      this->m_UpdateTitleFilterItemCount();

      // show or hide table and settings
      this->m_ShowNoFilter(this->mpc_TableModel->rowCount() == 0);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot of delete key button press signal from table view.

   Delete filter item on delete key press.

   \param[in]       os32_CurrentRow    currently selected row
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterPopup::m_OnDeleteKeyPressed(void) const
{
   // deletion is handled by index click so we call index click with index of current row and column "Remove"
   const QModelIndex c_Index =
      this->mpc_TableModel->index(this->m_GetCurrentSelectedRowIndex(),
                                  C_CamMosFilterTableModel::h_EnumToColumn(C_CamMosFilterTableModel::eREMOVE));

   this->m_OnIndexClicked(c_Index);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of current index/row change of table view (e.g. by arrow navigation).
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterPopup::m_OnRowChanged(const QModelIndex & orc_NewIndex, const QModelIndex & orc_PrevIndex) const
{
   Q_UNUSED(orc_PrevIndex)

   // update settings section
   const QList<C_CamProFilterItemData> & rc_Data = this->mpc_TableModel->GetFilterItemsData();
   const sint32 s32_SelectedRowIndex = orc_NewIndex.row();

   // if there is no selected index something really went wrong
   tgl_assert(s32_SelectedRowIndex < rc_Data.size());
   if ((s32_SelectedRowIndex >= 0) && (s32_SelectedRowIndex < rc_Data.size()))
   {
      this->m_UpdateSettingsSection(rc_Data[s32_SelectedRowIndex]);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot for combo box type selection.

   \param[in]     os32_NewType     new index (corresponds to type)
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterPopup::m_OnTypeChanged(const sint32 os32_NewType) const
{
   const sint32 s32_CurrentRowIndex = this->m_GetCurrentSelectedRowIndex();

   // if there is no selected index something really went wrong
   tgl_assert(s32_CurrentRowIndex >= 0);
   if (s32_CurrentRowIndex >= 0)
   {
      // update data in column type and not in actual selected column
      const QModelIndex c_Index = this->mpc_TableModel->index(
         s32_CurrentRowIndex, C_CamMosFilterTableModel::h_EnumToColumn(C_CamMosFilterTableModel::eTYPE));

      this->mpc_TableModel->setData(c_Index, static_cast<sintn>(os32_NewType), static_cast<sintn>(Qt::DisplayRole));

      // set right value to line edit "To"
      const QList<C_CamProFilterItemData> c_Data = this->mpc_TableModel->GetFilterItemsData();

      if (s32_CurrentRowIndex < c_Data.size())
      {
         const C_CamProFilterItemData c_FilterItem = c_Data[s32_CurrentRowIndex];
         this->mpc_Ui->pc_LeCanIdEnd->SetFromVariant(mh_GetValueAsHex(c_FilterItem.u32_EndId),
                                                     static_cast<uint64>(c_FilterItem.u32_EndId));
      }

      // GUI
      this->m_ShowTypeSpecificWidgets(os32_NewType);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of radio button "Pass" toggle.

   \param[in]     oq_Checked     true: pass, false: stop
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterPopup::m_OnRadioButtonToggle(const bool oq_Checked) const
{
   const sint32 s32_CurrentRowIndex = this->m_GetCurrentSelectedRowIndex();

   // if there is no selected index something really went wrong
   tgl_assert(s32_CurrentRowIndex >= 0);
   if (s32_CurrentRowIndex >= 0)
   {
      // update data in column mode and not in actual selected column
      const QModelIndex c_Index = this->mpc_TableModel->index(
         s32_CurrentRowIndex, C_CamMosFilterTableModel::h_EnumToColumn(C_CamMosFilterTableModel::eMODE));
      this->mpc_TableModel->setData(c_Index, oq_Checked, static_cast<sintn>(Qt::DisplayRole));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of line edit "From" editing finished signal.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterPopup::m_OnStartIdEdited(void)
{
   const sint32 s32_CurrentRowIndex = this->m_GetCurrentSelectedRowIndex();

   // if there is no selected index something really went wrong
   tgl_assert(s32_CurrentRowIndex >= 0);
   if (s32_CurrentRowIndex >= 0)
   {
      QString c_ErrorDescription;
      QVariant c_Value;

      if (this->mpc_Ui->pc_LeCanIdStart->GetValueAsVariant(c_Value, c_ErrorDescription) == C_NO_ERR)
      {
         // check if greater than end ID by comparing with data
         const QList<C_CamProFilterItemData> & rc_FilterItems =  this->mpc_TableModel->GetFilterItemsData();
         tgl_assert(s32_CurrentRowIndex < rc_FilterItems.size());
         if (s32_CurrentRowIndex < rc_FilterItems.size())
         {
            uint32 u32_EndId = rc_FilterItems[s32_CurrentRowIndex].u32_EndId;
            if (static_cast<uint32>(c_Value.toULongLong()) > u32_EndId)
            {
               // set end ID to same value
               u32_EndId = static_cast<uint32>(c_Value.toULongLong());
            }

            // set new ID(s)
            this->mpc_TableModel->SetFilterItemIDs(s32_CurrentRowIndex,
                                                   static_cast<uint32>(c_Value.toULongLong()), u32_EndId);
         }
      }
      else
      {
         // disconnect to ignore editingFinished signal on focus lose because of popup
         disconnect(this->mpc_Ui->pc_LeCanIdStart, &C_CamOgeLeIDPopUp::editingFinished, this,
                    &C_CamMosFilterPopup::m_OnStartIdEdited);
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eWARNING, c_ErrorDescription);
         c_Message.SetHeading(C_GtGetText::h_GetText("Invalid input"));
         c_Message.Execute();
         connect(this->mpc_Ui->pc_LeCanIdStart, &C_CamOgeLeIDPopUp::editingFinished, this,
                 &C_CamMosFilterPopup::m_OnStartIdEdited);
      }

      // (re)set GUI
      this->m_UpdateLineEdits(s32_CurrentRowIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of line edit "To" editing finished signal.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterPopup::m_OnEndIdEdited(void)
{
   const sint32 s32_CurrentRowIndex = this->m_GetCurrentSelectedRowIndex();

   // if there is no selected index something really went wrong
   tgl_assert(s32_CurrentRowIndex >= 0);
   if (s32_CurrentRowIndex >= 0)
   {
      QString c_ErrorDescription;
      QVariant c_Value;

      if (this->mpc_Ui->pc_LeCanIdEnd->GetValueAsVariant(c_Value, c_ErrorDescription) == C_NO_ERR)
      {
         // check if less than start ID by comparing with data
         const QList<C_CamProFilterItemData> & rc_FilterItems =  this->mpc_TableModel->GetFilterItemsData();
         tgl_assert(s32_CurrentRowIndex < rc_FilterItems.size());
         if (s32_CurrentRowIndex < rc_FilterItems.size())
         {
            uint32 u32_StartId = rc_FilterItems[s32_CurrentRowIndex].u32_StartId;
            if (static_cast<uint32>(c_Value.toULongLong()) < u32_StartId)
            {
               // set start ID to same value
               u32_StartId = static_cast<uint32>(c_Value.toULongLong());
            }

            // set new ID(s)
            this->mpc_TableModel->SetFilterItemIDs(s32_CurrentRowIndex, u32_StartId,
                                                   static_cast<uint32>(c_Value.toULongLong()));
         }
      }
      else
      {
         // disconnect to ignore editingFinished signal on focus lose because of popup
         disconnect(this->mpc_Ui->pc_LeCanIdEnd, &C_CamOgeLeIDPopUp::editingFinished, this,
                    &C_CamMosFilterPopup::m_OnEndIdEdited);
         C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eWARNING, c_ErrorDescription);
         c_Message.SetHeading(C_GtGetText::h_GetText("Invalid input"));
         c_Message.Execute();
         connect(this->mpc_Ui->pc_LeCanIdEnd, &C_CamOgeLeIDPopUp::editingFinished, this,
                 &C_CamMosFilterPopup::m_OnEndIdEdited);
      }

      // (re)set GUI
      this->m_UpdateLineEdits(s32_CurrentRowIndex);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of check box "extended" toggle.

   \param[in]     oq_Checked     true: extended, false: standard
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterPopup::m_OnExtendedToggled(const bool oq_Checked)
{
   const sint32 s32_CurrentRowIndex = this->m_GetCurrentSelectedRowIndex();

   // if there is no selected index something really went wrong
   tgl_assert(s32_CurrentRowIndex >= 0);
   if (s32_CurrentRowIndex >= 0)
   {
      this->mpc_TableModel->SetFilterItemExtended(s32_CurrentRowIndex, oq_Checked);
      // adapt maximum of IDs
      if (oq_Checked == true)
      {
         this->mpc_Ui->pc_LeCanIdStart->SetMaxFromVariant("0x1FFFFFFF");
         this->mpc_Ui->pc_LeCanIdEnd->SetMaxFromVariant("0x1FFFFFFF");
      }
      else
      {
         QString c_ErrorDescription;
         QVariant c_Value;

         this->mpc_Ui->pc_LeCanIdStart->SetMaxFromVariant("0x7FF");
         this->mpc_Ui->pc_LeCanIdEnd->SetMaxFromVariant("0x7FF");

         // when changing from extended to standard a range check is necessary
         if ((this->mpc_Ui->pc_LeCanIdStart->GetValueAsVariant(c_Value, c_ErrorDescription) != C_NO_ERR) ||
             (this->mpc_Ui->pc_LeCanIdEnd->GetValueAsVariant(c_Value, c_ErrorDescription) != C_NO_ERR))
         {
            // reset extended flag and inform user
            this->mpc_Ui->pc_CheckBoxExtended->setChecked(true);
            C_OgeWiCustomMessage c_Message(this, C_OgeWiCustomMessage::eWARNING);
            c_Message.SetHeading(C_GtGetText::h_GetText("Invalid Flag"));
            c_Message.SetDescription(
               C_GtGetText::h_GetText("Current CAN message ID is out of range of standard format. To remove the "
                                      "extended flag, first set CAN ID to 0x7FF or less."));
            c_Message.Execute();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of add from database button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterPopup::m_OnAddFromDatabase(void)
{
   QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
   C_CamMosDatabaseSelectionPopup * pc_Dialog = new C_CamMosDatabaseSelectionPopup(*c_New, false);

   //Resize
   c_New->SetSize(QSize(700, 800));

   // Update settings on accept
   if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
   {
      // Get selected data element
      const std::vector<std::array<QString, 2> > c_SelectedItems = pc_Dialog->GetSelectedDataElements();
      // Database file: c_SelectedItems[0][0]  and Message name: c_SelectedItems[0][1]

      // we used single-selection, just to make sure:
      if (c_SelectedItems.size() == 1)
      {
         const QFileInfo c_FileInfo(c_SelectedItems[0][0]);
         const QString c_MessageName = c_SelectedItems[0][1];
         if (c_FileInfo.suffix().compare("dbc", Qt::CaseInsensitive) == 0)
         {
            //Get data
            const C_CieConverter::C_CIECanMessage * const pc_Message = C_CamDbHandler::h_GetInstance()->GetDbcMessage(
               c_FileInfo.absoluteFilePath(), c_MessageName);

            // Update message of given CAN ID
            if (pc_Message != NULL)
            {
               this->m_SetMessageDataFromDatabase(pc_Message->u32_CanId, pc_Message->q_IsExtended);
            }
         }
         else if (c_FileInfo.suffix().compare("syde_sysdef", Qt::CaseInsensitive) == 0)
         {
            //First prepare C_CamDbHandler
            if (C_CamDbHandler::h_GetInstance()->FindOsyMessage(c_FileInfo.absoluteFilePath(),
                                                                c_MessageName) == C_NO_ERR)
            {
               //Get data
               const C_OSCCanMessage * const pc_Message = C_CamDbHandler::h_GetInstance()->GetOSCMessage(
                  c_FileInfo.absoluteFilePath(), c_MessageName);

               // Update message of given CAN ID
               if (pc_Message != NULL)
               {
                  this->m_SetMessageDataFromDatabase(pc_Message->u32_CanId, pc_Message->q_IsExtended);
               }
            }
         }
      }
   }

   if (c_New != NULL)
   {
      pc_Dialog->SaveUserSettings();
      c_New->HideOverlay();
   }
   //lint -e{429}  no memory leak because of the parent of pc_Dialog and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Return row index of currently selected row.

   \return
   row index (-1 if invalid selection)
*/
//----------------------------------------------------------------------------------------------------------------------
sint32 C_CamMosFilterPopup::m_GetCurrentSelectedRowIndex(void) const
{
   return this->mpc_Ui->pc_TableView->currentIndex().row();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update title: show number of databases
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterPopup::m_UpdateTitleFilterItemCount(void) const
{
   this->mpc_Ui->pc_LabelFilterItems->setText(
      QString(C_GtGetText::h_GetText("Filter Items (%1)")).arg(this->mpc_TableModel->rowCount()));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set Message ID and extended flag from database.

   Includes reset of extended flag if something went wrong. Detailed function description (optional).

   \param[in]       u32_CanId      Message ID
   \param[in]       oq_IsExtended  Extended flag
*/
//----------------------------------------------------------------------------------------------------------------------
void C_CamMosFilterPopup::m_SetMessageDataFromDatabase(const uint32 ou32_CanId, const bool oq_IsExtended)
{
   QVariant c_CanIdBefore;
   QVariant c_CanIdAfter;
   QString c_Errors = "";

   // remember previous values
   const bool q_WasExtended = this->mpc_Ui->pc_CheckBoxExtended->isChecked();
   tgl_assert(this->mpc_Ui->pc_LeCanIdStart->GetValueAsVariant(c_CanIdBefore, c_Errors) == C_NO_ERR);

   // set information in line edit and check box to use line edits min/max handling
   this->mpc_Ui->pc_CheckBoxExtended->setChecked(oq_IsExtended);
   this->mpc_Ui->pc_LeCanIdStart->setText(mh_GetValueAsHex(ou32_CanId));

   // trigger change
   this->m_OnStartIdEdited();

   // reset extended flag if ID did not change
   tgl_assert(this->mpc_Ui->pc_LeCanIdStart->GetValueAsVariant(c_CanIdAfter, c_Errors) == C_NO_ERR);
   if ((c_CanIdBefore == c_CanIdAfter) && (this->mpc_Ui->pc_CheckBoxExtended->isChecked() != q_WasExtended))
   {
      this->mpc_Ui->pc_CheckBoxExtended->setChecked(q_WasExtended);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Convert int value to string.
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_CamMosFilterPopup::mh_GetValueAsHex(const uint32 u32_Value)
{
   return "0x" + QString("%1").arg(u32_Value, 0, 16).toUpper();
}
