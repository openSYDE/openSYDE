//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for toolbox of dashboard

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     20.04.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include "C_SyvDaDashboardToolbox.h"
#include "ui_C_SyvDaDashboardToolbox.h"
#include "C_GtGetText.h"
#include "C_SebToolboxUtil.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_parent Optional pointer to parent

   \created     20.04.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvDaDashboardToolbox::C_SyvDaDashboardToolbox(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SyvDaDashboardToolbox)
{
   mpc_Ui->setupUi(this);

   this->mpc_Ui->pc_ScrollAreaWidgetsWidget->SetBackgroundColor(-1);
   this->mpc_Ui->pc_ScrollAreaFunctionBlocksWidget->SetBackgroundColor(-1);
   this->mpc_Ui->pc_ScrollAreaDrawingWidget->SetBackgroundColor(-1);

   InitStaticNames();
   m_FillToolboxStatic();

   // set the 'nodes' tab initially
   this->mpc_Ui->pc_TabWidget->setCurrentIndex(0);

   mpc_Ui->pc_ListWidgetDrawing->setDropIndicatorShown(false);
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     20.04.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SyvDaDashboardToolbox::~C_SyvDaDashboardToolbox()
{
   delete mpc_Ui;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize all displayed static names

   \created     03.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardToolbox::InitStaticNames(void) const
{
   this->mpc_Ui->pc_TabWidget->tabBar()->setTabText(0, C_GtGetText::h_GetText("Widgets"));
   this->mpc_Ui->pc_TabWidget->tabBar()->setTabText(1, C_GtGetText::h_GetText("Function Blocks"));
   this->mpc_Ui->pc_TabWidget->tabBar()->setTabText(2, C_GtGetText::h_GetText("Meta"));
   this->mpc_Ui->pc_LabelFreeElements->setText(C_GtGetText::h_GetText("Meta Information Elements"));
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the dark mode

   \param[in] oq_Active Dark mode active

   \created     04.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardToolbox::ApplyDarkMode(const bool oq_Active)
{
   for (uint32 u32_ItList = 0; u32_ItList < static_cast<uint32>(this->mc_ListWidgets.size()); ++u32_ItList)
   {
      C_SdTopologyListWidget * const pc_ListWidget = this->mc_ListWidgets[u32_ItList];
      if (pc_ListWidget != NULL)
      {
         pc_ListWidget->ApplyDarkMode(oq_Active);
      }
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten enter event slot

   To get the correct cursor back from the resizing implementation of
   C_OgeWiHover.

   \param[in,out] opc_Event Event identification and information

   \created     03.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardToolbox::enterEvent(QEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)

   this->setCursor(Qt::ArrowCursor);
   this->parentWidget()->setMouseTracking(false);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten leave event slot

   To get the correct cursor back from the resizing implementation of
   C_OgeWiHover.

   \param[in,out] opc_Event Event identification and information

   \created     03.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardToolbox::leaveEvent(QEvent * const opc_Event)
{
   Q_UNUSED(opc_Event)

   this->parentWidget()->setMouseTracking(true);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Fill toolbox with static content

   \created     03.08.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardToolbox::m_FillToolboxStatic(void)
{
   C_SdTopologyListWidget * pc_List;

   //Free elements
   //-------------
   C_SebToolboxUtil::h_InitFreeElements(this->mpc_Ui->pc_ListWidgetDrawing);

   //Function blocks
   //---------------
   pc_List = C_SebToolboxUtil::h_AddNewList(C_GtGetText::h_GetText("Parametrization"), this->mpc_Ui->pc_VerticalLayout3,
                                            this->mc_ListWidgets, this);
   C_SebToolboxUtil::h_AddElementToList(pc_List, C_GtGetText::h_GetText(
                                           "Parametrization Widget"),
                                        "://images/system_views/dashboards/function_blocks/PreviewParamWidget.svg",
                                        "://images/system_views/dashboards/function_blocks/PreviewParamWidgetDark.svg",
                                        C_GtGetText::h_GetText(
                                           "This widget is used to read data from and write data to the NVM memory of the ECU reference over the NVM Datapools."),
                                        C_GtGetText::h_GetText("Parametrization Widget"));

   // add a final spacer
   C_SebToolboxUtil::h_AddFinalSpacer(this->mpc_Ui->pc_VerticalLayout3, pc_List);

   //Drawing items
   //-------------
   m_AddWidgetItems();

   // configure Headings
   this->mpc_Ui->pc_LabelFreeElements->setMaximumHeight(C_SebToolboxUtil::hsn_LabelSize);
   this->mpc_Ui->pc_LabelFreeElements->setMinimumHeight(C_SebToolboxUtil::hsn_LabelSize);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Add widget items to toolbox

   \created     23.10.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaDashboardToolbox::m_AddWidgetItems(void)
{
   //No global static variable because gettext needs to be initialized first
   const QString c_Theme1 = C_GtGetText::h_GetText("openSYDE");
   const QString c_Theme2 = C_GtGetText::h_GetText("Material");
   const QString c_Theme3 = C_GtGetText::h_GetText("Flat");
   const QString c_Theme4 = C_GtGetText::h_GetText("Skeuomorphic");
   const QString c_ToolTipPrefix = C_GtGetText::h_GetText(" Theme");
   C_SdTopologyListWidget * pc_List;

   //not supported yet
   //   pc_List = C_SebToolboxUtil::h_AddNewList(C_GtGetText::h_GetText(
   //                                               "Combo-Box"), this->mpc_Ui->pc_VerticalLayout1,
   // this->mc_ListWidgets,
   //                                            this);
   //   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme1, "://images/system_views/dashboards/ComboBoxStw.png",
   //                                        "://images/system_views/dashboards/ComboBoxStwDark.png", c_Theme1);
   //   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme2, "://images/system_views/dashboards/ComboBoxStw2.png",
   //                                        "://images/system_views/dashboards/ComboBoxStw2Dark.png", c_Theme2);
   //   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme3, "://images/system_views/dashboards/ComboBoxFlat.png",
   //                                        "://images/system_views/dashboards/ComboBoxFlatDark.png", c_Theme3);
   //   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme4, "://images/system_views/dashboards/ComboBoxSkeu.png",
   //                                        "://images/system_views/dashboards/ComboBoxSkeuDark.png", c_Theme4);

   C_SebToolboxUtil::h_AddNewHeading(C_GtGetText::h_GetText(
                                        "Reading Widgets"), this->mpc_Ui->pc_VerticalLayout1, this, false);
   pc_List = C_SebToolboxUtil::h_AddNewList(C_GtGetText::h_GetText(
                                               "Value Label"), this->mpc_Ui->pc_VerticalLayout1, this->mc_ListWidgets,
                                            this);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme1, "://images/system_views/dashboards/ValueLabelStw.png",
                                        "://images/system_views/dashboards/ValueLabelStwDark.png",
                                        c_Theme1 + c_ToolTipPrefix);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme2, "://images/system_views/dashboards/ValueLabelStw2.png",
                                        "://images/system_views/dashboards/ValueLabelStw2Dark.png",
                                        c_Theme2 + c_ToolTipPrefix);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme3, "://images/system_views/dashboards/ValueLabelFlat.png",
                                        "://images/system_views/dashboards/ValueLabelFlatDark.png",
                                        c_Theme3 + c_ToolTipPrefix);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme4, "://images/system_views/dashboards/ValueLabelSkeu.png",
                                        "://images/system_views/dashboards/ValueLabelSkeuDark.png",
                                        c_Theme4 + c_ToolTipPrefix);

   pc_List = C_SebToolboxUtil::h_AddNewList(C_GtGetText::h_GetText(
                                               "Pie Chart"), this->mpc_Ui->pc_VerticalLayout1, this->mc_ListWidgets,
                                            this);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme1, "://images/system_views/dashboards/PieChartStw.png",
                                        "://images/system_views/dashboards/PieChartStwDark.png",
                                        c_Theme1 + c_ToolTipPrefix);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme2, "://images/system_views/dashboards/PieChartStw2.png",
                                        "://images/system_views/dashboards/PieChartStw2Dark.png",
                                        c_Theme2 + c_ToolTipPrefix);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme3, "://images/system_views/dashboards/PieChartFlat.png",
                                        "://images/system_views/dashboards/PieChartFlatDark.png",
                                        c_Theme3 + c_ToolTipPrefix);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme4, "://images/system_views/dashboards/PieChartSkeu.png",
                                        "://images/system_views/dashboards/PieChartSkeuDark.png",
                                        c_Theme4 + c_ToolTipPrefix);

   pc_List = C_SebToolboxUtil::h_AddNewList(C_GtGetText::h_GetText(
                                               "Bar"), this->mpc_Ui->pc_VerticalLayout1, this->mc_ListWidgets,
                                            this);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme1, "://images/system_views/dashboards/ProgressBarHorStw.png",
                                        "://images/system_views/dashboards/ProgressBarHorStwDark.png",
                                        c_Theme1 + c_ToolTipPrefix);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme2, "://images/system_views/dashboards/ProgressBarHorStw2.png",
                                        "://images/system_views/dashboards/ProgressBarHorStw2Dark.png",
                                        c_Theme2 + c_ToolTipPrefix);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme3, "://images/system_views/dashboards/ProgressBarHorFlat.png",
                                        "://images/system_views/dashboards/ProgressBarHorFlatDark.png",
                                        c_Theme3 + c_ToolTipPrefix);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme4, "://images/system_views/dashboards/ProgressBarHorSkeu.png",
                                        "://images/system_views/dashboards/ProgressBarHorSkeuDark.png",
                                        c_Theme4 + c_ToolTipPrefix);

   pc_List = C_SebToolboxUtil::h_AddNewList(C_GtGetText::h_GetText(
                                               "Table"), this->mpc_Ui->pc_VerticalLayout1, this->mc_ListWidgets,
                                            this);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme1, "://images/system_views/dashboards/DataListStw.png",
                                        "://images/system_views/dashboards/DataListStwDark.png",
                                        c_Theme1 + c_ToolTipPrefix);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme2, "://images/system_views/dashboards/DataListStw2.png",
                                        "://images/system_views/dashboards/DataListStw2Dark.png",
                                        c_Theme2 + c_ToolTipPrefix);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme3, "://images/system_views/dashboards/DataListFlat.png",
                                        "://images/system_views/dashboards/DataListFlatDark.png",
                                        c_Theme3 + c_ToolTipPrefix);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme4, "://images/system_views/dashboards/DataListSkeu.png",
                                        "://images/system_views/dashboards/DataListSkeuDark.png",
                                        c_Theme4 + c_ToolTipPrefix);

   pc_List = C_SebToolboxUtil::h_AddNewList(C_GtGetText::h_GetText(
                                               "2D Chart"), this->mpc_Ui->pc_VerticalLayout1, this->mc_ListWidgets,
                                            this);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme1, "://images/system_views/dashboards/TDChartStw.png",
                                        "://images/system_views/dashboards/TDChartStwDark.png",
                                        c_Theme1 + c_ToolTipPrefix);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme2, "://images/system_views/dashboards/TDChartStw2.png",
                                        "://images/system_views/dashboards/TDChartStw2Dark.png",
                                        c_Theme2 + c_ToolTipPrefix);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme3, "://images/system_views/dashboards/TDChartFlat.png",
                                        "://images/system_views/dashboards/TDChartFlatDark.png",
                                        c_Theme3 + c_ToolTipPrefix);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme4, "://images/system_views/dashboards/TDChartSkeu.png",
                                        "://images/system_views/dashboards/TDChartSkeuDark.png",
                                        c_Theme4 + c_ToolTipPrefix);

   //add free space
   C_SebToolboxUtil::h_AddNewHeading(C_GtGetText::h_GetText(
                                        "Writing Widgets"), this->mpc_Ui->pc_VerticalLayout1, this, true);

   pc_List = C_SebToolboxUtil::h_AddNewList(C_GtGetText::h_GetText(
                                               "Toggle"), this->mpc_Ui->pc_VerticalLayout1, this->mc_ListWidgets,
                                            this);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme1, "://images/system_views/dashboards/ToggleStw.png",
                                        "://images/system_views/dashboards/ToggleStwDark.png",
                                        c_Theme1 + c_ToolTipPrefix);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme2, "://images/system_views/dashboards/ToggleStw2.png",
                                        "://images/system_views/dashboards/ToggleStw2Dark.png",
                                        c_Theme2 + c_ToolTipPrefix);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme3, "://images/system_views/dashboards/ToggleFlat.png",
                                        "://images/system_views/dashboards/ToggleFlatDark.png",
                                        c_Theme3 + c_ToolTipPrefix);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme4, "://images/system_views/dashboards/ToggleSkeu.png",
                                        "://images/system_views/dashboards/ToggleSkeuDark.png",
                                        c_Theme4 + c_ToolTipPrefix);

   pc_List = C_SebToolboxUtil::h_AddNewList(C_GtGetText::h_GetText(
                                               "Slider"), this->mpc_Ui->pc_VerticalLayout1, this->mc_ListWidgets,
                                            this);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme1, "://images/system_views/dashboards/SliderStw.png",
                                        "://images/system_views/dashboards/SliderStwDark.png",
                                        c_Theme1 + c_ToolTipPrefix);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme2, "://images/system_views/dashboards/SliderStw2.png",
                                        "://images/system_views/dashboards/SliderStw2Dark.png",
                                        c_Theme2 + c_ToolTipPrefix);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme3, "://images/system_views/dashboards/SliderFlat.png",
                                        "://images/system_views/dashboards/SliderFlatDark.png",
                                        c_Theme3 + c_ToolTipPrefix);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme4, "://images/system_views/dashboards/SliderSkeu.png",
                                        "://images/system_views/dashboards/SliderSkeuDark.png",
                                        c_Theme4 + c_ToolTipPrefix);

   pc_List = C_SebToolboxUtil::h_AddNewList(C_GtGetText::h_GetText(
                                               "Spinbox"), this->mpc_Ui->pc_VerticalLayout1, this->mc_ListWidgets,
                                            this);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme1, "://images/system_views/dashboards/SpinBoxStw.png",
                                        "://images/system_views/dashboards/SpinBoxStwDark.png",
                                        c_Theme1 + c_ToolTipPrefix);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme2, "://images/system_views/dashboards/SpinBoxStw2.png",
                                        "://images/system_views/dashboards/SpinBoxStw2Dark.png",
                                        c_Theme2 + c_ToolTipPrefix);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme3, "://images/system_views/dashboards/SpinBoxFlat.png",
                                        "://images/system_views/dashboards/SpinBoxFlatDark.png",
                                        c_Theme3 + c_ToolTipPrefix);
   C_SebToolboxUtil::h_AddElementToList(pc_List, c_Theme4, "://images/system_views/dashboards/SpinBoxSkeu.png",
                                        "://images/system_views/dashboards/SpinBoxSkeuDark.png",
                                        c_Theme4 + c_ToolTipPrefix);

   // add a final spacer
   C_SebToolboxUtil::h_AddFinalSpacer(this->mpc_Ui->pc_VerticalLayout1, pc_List);
}
