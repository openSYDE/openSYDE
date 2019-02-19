//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for system view dashboard widget properties (implementation)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     05.09.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <climits>
#include <QVBoxLayout>

#include "TGLUtils.h"
#include "stwerrors.h"
#include "C_SyvUtil.h"
#include "C_OgeWiUtil.h"
#include "C_GtGetText.h"
#include "C_SyvDaPeBase.h"
#include "C_GiSvDaRectBaseGroup.h"
#include "C_SyvDaPeUpdateModeConfiguration.h"
#include "C_SyvDaPeDataElementBrowse.h"
#include "C_PuiSdHandler.h"
#include "C_PuiSvHandler.h"
#include "ui_C_SyvDaPeBase.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;
using namespace stw_opensyde_gui_logic;
using namespace stw_opensyde_gui_elements;

/* -- Module Global Constants ---------------------------------------------- */
const sintn C_SyvDaPeBase::mhsn_INDEX_MANUAL = 0;
const sintn C_SyvDaPeBase::mhsn_INDEX_ON_CHANGE = 1;
const sintn C_SyvDaPeBase::mhsn_INDEX_THEME_OPENSYDE = 0;
const sintn C_SyvDaPeBase::mhsn_INDEX_THEME_OPENSYDE_2 = 1;
const sintn C_SyvDaPeBase::mhsn_INDEX_THEME_FLAT = 2;
const sintn C_SyvDaPeBase::mhsn_INDEX_THEME_SKEUMORPH = 3;

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] orc_Parent                  Reference to parent
   \param[in]     ou32_ViewIndex              View index
   \param[in]     ou32_DashboardIndex         Dashboard index
   \param[in]     orc_Name                    Name of the item for the title
   \param[in]     orc_Id                      Element id to use for default initialization (only used if not invalid)
   \param[in]     rc_Scaling                  Scaling information for default initialization
   \param[in]     oq_ReadElement              Optional flag if dialog for read element
   \param[in]     oq_DarkMode                 Optional flag if dark mode active
   \param[in]     oq_ShowWidgetSpecificPart   Optional flag if widget specific part is visible
   \param[in]     oq_AllowChangeOfDataElement Optional flag if data element may be changed
   \param[in]     orc_DisplayName             Optional display name (only used if element name cannot be changed)

   \created     05.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaPeBase::C_SyvDaPeBase(C_OgePopUpDialog & orc_Parent, const uint32 ou32_ViewIndex,
                             const uint32 ou32_DashboardIndex, const QString & orc_Name,
                             const C_PuiSvDbNodeDataPoolListElementId & orc_Id,
                             const C_PuiSvDbDataElementScaling & rc_Scaling, const bool oq_ReadElement,
                             const bool oq_DarkMode, const bool oq_ShowWidgetSpecificPart,
                             const bool oq_AllowChangeOfDataElement, const QString & orc_DisplayName) :
   QWidget(&orc_Parent),
   mpc_Ui(new Ui::C_SyvDaPeBase),
   mpc_ParentDialog(&orc_Parent),
   mc_DataElement(orc_Id),
   mc_InitialDataElement(orc_Id),
   mu32_ViewIndex(ou32_ViewIndex),
   mu32_DashboardIndex(ou32_DashboardIndex),
   mq_NewConfigAdded(false),
   mq_ConfigRestorationNecessary(false),
   mq_ReadElement(oq_ReadElement),
   mc_DisplayName(orc_DisplayName)
{
   const QSize c_Size = h_GetSceneViewSize();

   mpc_Ui->setupUi(this);

   // register the widget for showing
   this->mpc_ParentDialog->SetWidget(this);

   // set main title
   this->mpc_ParentDialog->SetTitle(QString(C_GtGetText::h_GetText("%1 Widget")).arg(orc_Name));

   //Factor needs to be above zero
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Ui->pc_DoubleSpinBoxFactor->SetMinimumCustom(C_OgeSpxFactor::mhf64_FACTOR_MIN);

   //Debug texts
   this->mpc_Ui->pc_GroupBoxWrite->setTitle("");
   this->mpc_Ui->pc_GroupBoxRead->setTitle("");

   //Type
   if (oq_ReadElement == true)
   {
      this->mpc_Ui->pc_GroupBoxWrite->setVisible(false);
   }
   else
   {
      this->mpc_Ui->pc_GroupBoxRead->setVisible(false);
   }

   //Deactivate certain fields
   this->mpc_Ui->pc_LineEditDataElement->setReadOnly(true);

   //Init date element fields
   if (orc_Id.GetIsValid() == true)
   {
      m_InitDataElement(orc_Id, rc_Scaling);
   }
   else
   {
      m_InitNoDataElement();
   }

   // create scene for graphics view
   this->mpc_Scene = new C_SyvDaDashboardScene(0, 0, false, NULL);

   // configure background drawing
   this->mpc_Scene->setSceneRect(0.0, 0.0,
                                 static_cast<float64>(c_Size.width()),
                                 static_cast<float64>(c_Size.height()));
   this->mpc_Ui->pc_GraphicsView->setScene(this->mpc_Scene);
   this->mpc_Ui->pc_GraphicsView->setEnabled(false);
   this->mpc_Ui->pc_GraphicsView->SetSubtleSurroundGradient(true);
   this->mpc_Scene->SetDarkModeActive(oq_DarkMode);
   this->mpc_Ui->pc_GraphicsView->SetDarkMode(oq_DarkMode);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_GroupBox, "Edit", true);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_GroupBox, "DarkMode", oq_DarkMode);
   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_PreviewFrame, "NoBorder", oq_DarkMode);

   this->mpc_Ui->pc_GroupBoxWidgetSpecific->setVisible(oq_ShowWidgetSpecificPart);
   if (oq_AllowChangeOfDataElement == true)
   {
      this->mpc_Ui->pc_GroupBoxDataElementChangeable->setVisible(true);
      this->mpc_Ui->pc_GroupBoxDataElementDisplay->setVisible(false);
   }
   else
   {
      this->mpc_Ui->pc_GroupBoxDataElementChangeable->setVisible(false);
      this->mpc_Ui->pc_GroupBoxDataElementDisplay->setVisible(true);
   }

   InitStaticNames();

   //Spin box
   //lint -e{10,530,747,1015,1013}  c++11 feature
   this->mpc_Ui->pc_DoubleSpinBoxOffset->SetMinimumCustom(std::numeric_limits<float64>::lowest());
   this->mpc_Ui->pc_DoubleSpinBoxOffset->SetMaximumCustom(std::numeric_limits<float64>::max());

   // connects
   connect(this->mpc_Ui->pc_BushButtonOk, &QPushButton::clicked, this, &C_SyvDaPeBase::m_OkClicked);
   connect(this->mpc_Ui->pc_BushButtonCancel, &QPushButton::clicked,
           this, &C_SyvDaPeBase::m_CancelClicked);
   connect(this->mpc_Ui->pc_PushButtonDataElement, &QPushButton::clicked, this, &C_SyvDaPeBase::m_Browse);
   connect(this->mpc_Ui->pc_PushButtonUpdateModeConfigure, &QPushButton::clicked, this,
           &C_SyvDaPeBase::m_Configuration);
   connect(this->mpc_Ui->pc_CheckBoxDefaultScaling, &C_OgeChxProperties::toggled, this,
           &C_SyvDaPeBase::m_OnUseDefaultScalingChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxTheme, static_cast<void (QComboBox::*)(
                                                          sintn)>(&C_OgeCbxText::currentIndexChanged), this,
           &C_SyvDaPeBase::SigRefresh);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     05.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaPeBase::~C_SyvDaPeBase(void)
{
   delete mpc_Ui;
   delete mpc_Scene;
   //lint -e{1740}  no memory leak because the ownership of these objects was never transfered to this class
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize all displayed static names

   \created     05.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaPeBase::InitStaticNames(void)
{
   this->mpc_ParentDialog->SetSubTitle(C_GtGetText::h_GetText("Properties"));
   this->mpc_Ui->pc_BushButtonOk->setText(C_GtGetText::h_GetText("OK"));
   this->mpc_Ui->pc_BushButtonCancel->setText(C_GtGetText::h_GetText("Cancel"));
   this->mpc_Ui->pc_LabelDesign->setText(C_GtGetText::h_GetText("Design"));
   this->mpc_Ui->pc_LabelPreview->setText(C_GtGetText::h_GetText("Example Preview"));
   this->mpc_Ui->pc_LabelDataElement->setText(C_GtGetText::h_GetText("Data Element"));
   this->mpc_Ui->pc_LabelUpdateModeHeading->setText(C_GtGetText::h_GetText("Update Mode"));
   this->mpc_Ui->pc_LabelScaling->setText(C_GtGetText::h_GetText("Value Scaling"));
   this->mpc_Ui->pc_LabelFactor->setText(C_GtGetText::h_GetText("Factor"));
   this->mpc_Ui->pc_LabelOffset->setText(C_GtGetText::h_GetText("Offset"));
   this->mpc_Ui->pc_LabelUnit->setText(C_GtGetText::h_GetText("Unit"));
   this->mpc_Ui->pc_LabelNamespaceDescription->setText(C_GtGetText::h_GetText("Source"));
   this->mpc_Ui->pc_CheckBoxDefaultScaling->setText(C_GtGetText::h_GetText("Use the default values of SYSTEM DEFINITION"));
   this->mpc_Ui->pc_PushButtonUpdateModeConfigure->setText(C_GtGetText::h_GetText("Configure"));
   this->mpc_Ui->pc_ComboBoxTransmissionMode->addItem(C_GtGetText::h_GetText("On Trigger"));
   this->mpc_Ui->pc_ComboBoxTransmissionMode->addItem(C_GtGetText::h_GetText("On Change"));
   this->mpc_Ui->pc_PushButtonDataElement->setText(C_GtGetText::h_GetText("..."));
   this->mpc_Ui->pc_LabelTheme->setText(C_GtGetText::h_GetText("Theme"));
   this->mpc_Ui->pc_LabelDisplayName->setText(C_GtGetText::h_GetText("Display as"));
   this->mpc_Ui->pc_ComboBoxTheme->addItem(C_GtGetText::h_GetText("openSYDE"));
   this->mpc_Ui->pc_ComboBoxTheme->addItem(C_GtGetText::h_GetText("Material"));
   this->mpc_Ui->pc_ComboBoxTheme->addItem(C_GtGetText::h_GetText("Flat"));
   this->mpc_Ui->pc_ComboBoxTheme->addItem(C_GtGetText::h_GetText("Skeuomorphic"));
}

//-----------------------------------------------------------------------------
/*!
   \brief  Place widget in pop up dialog

   \param[in,out] opc_Widget Widget to place in pop up dialog

   \created     05.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaPeBase::SetWidget(QWidget * const opc_Widget)
{
   if (opc_Widget != NULL)
   {
      opc_Widget->setParent(this);
      this->mpc_Ui->pc_VerticalLayoutDesign->insertWidget(1, opc_Widget, 1);
      //lint -e{429}  no memory leak because of the parent of pc_Layout and the Qt memory management
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Sets the write mode

   \param[in]     oe_WriteMode      New write mode

   \created     12.09.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SyvDaPeBase::SetWriteMode(const C_PuiSvDbWidgetBase::E_WriteMode oe_WriteMode) const
{
   switch (oe_WriteMode)
   {
   case C_PuiSvDbWidgetBase::eWM_MANUAL:
      this->mpc_Ui->pc_ComboBoxTransmissionMode->setCurrentIndex(C_SyvDaPeBase::mhsn_INDEX_MANUAL);
      break;
   case C_PuiSvDbWidgetBase::eWM_ON_CHANGE:
      this->mpc_Ui->pc_ComboBoxTransmissionMode->setCurrentIndex(C_SyvDaPeBase::mhsn_INDEX_ON_CHANGE);
      break;
   default:
      this->mpc_Ui->pc_ComboBoxTransmissionMode->setCurrentIndex(C_SyvDaPeBase::mhsn_INDEX_MANUAL);
      break;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set theme

   \param[in] oe_Style Current theme

   \created     12.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaPeBase::SetTheme(const C_PuiSvDbWidgetBase::E_Style oe_Style) const
{
   switch (oe_Style)
   {
   case C_PuiSvDbWidgetBase::eOPENSYDE:
      this->mpc_Ui->pc_ComboBoxTheme->setCurrentIndex(C_SyvDaPeBase::mhsn_INDEX_THEME_OPENSYDE);
      break;
   case C_PuiSvDbWidgetBase::eOPENSYDE_2:
      this->mpc_Ui->pc_ComboBoxTheme->setCurrentIndex(C_SyvDaPeBase::mhsn_INDEX_THEME_OPENSYDE_2);
      break;
   case C_PuiSvDbWidgetBase::eSKEUOMORPH:
      this->mpc_Ui->pc_ComboBoxTheme->setCurrentIndex(C_SyvDaPeBase::mhsn_INDEX_THEME_SKEUMORPH);
      break;
   case C_PuiSvDbWidgetBase::eFLAT:
      this->mpc_Ui->pc_ComboBoxTheme->setCurrentIndex(C_SyvDaPeBase::mhsn_INDEX_THEME_FLAT);
      break;
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Returns the preview scene

   \return     Pointer to preview scene

   \created     05.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaDashboardScene * C_SyvDaPeBase::GetPreviewScene(void)
{
   return this->mpc_Scene;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get scene view size for improved item placement

   \return
   Scene view size

   \created     05.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QSize C_SyvDaPeBase::h_GetSceneViewSize(void)
{
   return QSize(259, 146);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get offset for icon placement

   \return
   Offset for icon placement

   \created     10.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
float64 C_SyvDaPeBase::h_GetFixIconOffset(void)
{
   return 25.0;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get ideal item size according to current scene size

   \param[out] orc_ItemPos  Ideal item position
   \param[out] orc_ItemSize Ideal item size
   \param[in]  opc_Item     Graphics item

   \created     10.08.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaPeBase::h_GetIdealItemRect(QPointF & orc_ItemPos, QSizeF & orc_ItemSize,
                                       const QGraphicsItem * const opc_Item)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   const C_GiSvDaRectBaseGroup * const pc_GiItem = dynamic_cast<const C_GiSvDaRectBaseGroup * const>(opc_Item);

   if (pc_GiItem != NULL)
   {
      const QSize c_ViewSize = C_SyvDaPeBase::h_GetSceneViewSize();
      //To be really centered we also have to include the borders into the size
      const float64 f64_DoubleInteractionPointOffset =
         (pc_GiItem->boundingRect().width() - pc_GiItem->GetVisibleBoundingRect().width());
      //Also include the fix offset to the right
      const float64 f64_IconOffset = C_SyvDaPeBase::h_GetFixIconOffset();
      orc_ItemSize = QSizeF(
         ((static_cast<float64>(c_ViewSize.width()) / 1.5) + f64_IconOffset) + f64_DoubleInteractionPointOffset,
         (static_cast<float64>(c_ViewSize.height()) / 1.5) + f64_DoubleInteractionPointOffset);
      orc_ItemPos = QPointF(
         ((static_cast<float64>(c_ViewSize.width()) - orc_ItemSize.width()) / 2.0) + (f64_IconOffset / 2.0),
         (static_cast<float64>(c_ViewSize.height()) - orc_ItemSize.height()) / 2.0);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get current data element id

   \return
   Current data element id

   \created     07.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSvDbNodeDataPoolListElementId C_SyvDaPeBase::GetDataElementId(void) const
{
   return this->mc_DataElement;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get current scaling information

   \return
   Current scaling information

   \created     08.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSvDbDataElementScaling C_SyvDaPeBase::GetScalingInformation() const
{
   C_PuiSvDbDataElementScaling c_Retval;

   c_Retval.q_UseDefault = this->mpc_Ui->pc_CheckBoxDefaultScaling->isChecked();
   c_Retval.f64_Offset = this->mpc_Ui->pc_DoubleSpinBoxOffset->value();
   c_Retval.f64_Factor = this->mpc_Ui->pc_DoubleSpinBoxFactor->value();
   c_Retval.c_Unit = this->mpc_Ui->pc_LineEditUnit->text();
   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get current write mode

   \return
   Current write mode

   \created     12.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSvDbWidgetBase::E_WriteMode C_SyvDaPeBase::GetWriteMode(void) const
{
   C_PuiSvDbWidgetBase::E_WriteMode e_Retval;
   switch (this->mpc_Ui->pc_ComboBoxTransmissionMode->currentIndex())
   {
   case C_SyvDaPeBase::mhsn_INDEX_MANUAL:
      e_Retval = C_PuiSvDbWidgetBase::eWM_MANUAL;
      break;
   case C_SyvDaPeBase::mhsn_INDEX_ON_CHANGE:
      e_Retval = C_PuiSvDbWidgetBase::eWM_ON_CHANGE;
      break;
   default:
      e_Retval = C_PuiSvDbWidgetBase::eWM_MANUAL;
      break;
   }
   return e_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get current theme

   \return
   Current theme

   \created     12.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_PuiSvDbWidgetBase::E_Style C_SyvDaPeBase::GetTheme(void) const
{
   C_PuiSvDbWidgetBase::E_Style e_Retval;
   switch (this->mpc_Ui->pc_ComboBoxTheme->currentIndex())
   {
   case C_SyvDaPeBase::mhsn_INDEX_THEME_OPENSYDE:
      e_Retval = C_PuiSvDbWidgetBase::eOPENSYDE;
      break;
   case C_SyvDaPeBase::mhsn_INDEX_THEME_OPENSYDE_2:
      e_Retval = C_PuiSvDbWidgetBase::eOPENSYDE_2;
      break;
   case C_SyvDaPeBase::mhsn_INDEX_THEME_FLAT:
      e_Retval = C_PuiSvDbWidgetBase::eFLAT;
      break;
   case C_SyvDaPeBase::mhsn_INDEX_THEME_SKEUMORPH:
      e_Retval = C_PuiSvDbWidgetBase::eSKEUOMORPH;
      break;
   default:
      e_Retval = C_PuiSvDbWidgetBase::eOPENSYDE;
      break;
   }
   return e_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get current display name

   \return
   Current display name

   \created     15.06.2018  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
QString C_SyvDaPeBase::GetDisplayName(void) const
{
   QString c_Retval = this->mpc_Ui->pc_LineEditDataElementDisplayName->text();

   if (this->mc_DataElement.GetIsValid() == true)
   {
      const C_OSCNodeDataPoolListElement * const pc_Element =
         C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(this->mc_DataElement.u32_NodeIndex,
                                                                    this->mc_DataElement.u32_DataPoolIndex,
                                                                    this->mc_DataElement.u32_ListIndex,
                                                                    this->mc_DataElement.u32_ElementIndex);
      if (pc_Element != NULL)
      {
         if (c_Retval.compare(pc_Element->c_Name.c_str()) == 0)
         {
            c_Retval = "";
         }
      }
   }

   return c_Retval;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out] opc_KeyEvent Event identification and information

   \created     05.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaPeBase::keyPressEvent(QKeyEvent * const opc_KeyEvent)
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
         if (this->mpc_ParentDialog != NULL)
         {
            this->mpc_ParentDialog->accept();
         }
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

//-----------------------------------------------------------------------------
/*!
   \brief   Slot of Ok button click

   \created     05.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaPeBase::m_OkClicked(void)
{
   if (this->mpc_ParentDialog != NULL)
   {
      this->mpc_ParentDialog->accept();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot of Cancel button

   \created     05.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaPeBase::m_CancelClicked(void)
{
   //Clean up last config if necessary
   if (this->mq_NewConfigAdded == true)
   {
      tgl_assert(C_PuiSvHandler::h_GetInstance()->RemoveViewReadRailItem(this->mu32_ViewIndex,
                                                                         this->mc_DataElement) == C_NO_ERR);
   }
   //Restore initial configuration
   if (this->mq_ConfigRestorationNecessary == true)
   {
      tgl_assert(C_PuiSvHandler::h_GetInstance()->AddViewReadRailItem(this->mu32_ViewIndex, this->mc_OriginalConfigId,
                                                                      this->mc_OriginalConfigData) == C_NO_ERR);
   }
   if (this->mpc_ParentDialog != NULL)
   {
      this->mpc_ParentDialog->reject();
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot of browse button

   \created     06.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaPeBase::m_Browse(void)
{
   //Set parent for better hierarchy handling via window manager
   QPointer<C_OgePopUpDialog> const c_New = new C_OgePopUpDialog(this, this);
   C_SyvDaPeDataElementBrowse * const pc_Dialog = new C_SyvDaPeDataElementBrowse(*c_New, this->mu32_ViewIndex, false,
                                                                                 !this->mq_ReadElement, false, true,
                                                                                 false);

   //Resize
   c_New->SetSize(QSize(800, 800));

   if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
   {
      const std::vector<C_PuiSvDbNodeDataPoolListElementId> c_DataElements = pc_Dialog->GetSelectedDataElements();
      //Only accept valid selection (assumed if any elements
      if (c_DataElements.size() > 0)
      {
         if (this->mq_ReadElement == true)
         {
            //Clean up last config if necessary
            if (this->mq_NewConfigAdded == true)
            {
               this->mq_NewConfigAdded = false;
               tgl_assert(C_PuiSvHandler::h_GetInstance()->RemoveViewReadRailItem(this->mu32_ViewIndex,
                                                                                  this->mc_DataElement) == C_NO_ERR);
            }
            else
            {
               //Only clean up if last selection was: valid and datapool element
               if ((this->mc_DataElement.GetIsValid() == true) &&
                   (this->mc_DataElement.GetType() == C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT))
               {
                  const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
                  //Save initial configuration
                  this->mc_OriginalConfigId = this->mc_DataElement;
                  tgl_assert(pc_View != NULL);
                  if (pc_View != NULL)
                  {
                     const QMap<C_OSCNodeDataPoolListElementId,
                                C_PuiSvReadDataConfiguration>::const_iterator c_ItResult =
                        pc_View->GetReadRailAssignments().find(this->mc_OriginalConfigId);
                     tgl_assert(c_ItResult != pc_View->GetReadRailAssignments().end());
                     if (c_ItResult != pc_View->GetReadRailAssignments().end())
                     {
                        this->mc_OriginalConfigData = c_ItResult.value();
                        const uint32 u32_Usages = pc_View->CountReadUsage(this->mc_DataElement);
                        //Allow delete if either no usages
                        // or only usage is by this element (assumed based on initial ID)
                        if ((u32_Usages == 0) ||
                            ((u32_Usages == 1) && (this->mc_DataElement == this->mc_InitialDataElement)))
                        {
                           //Remove initial configuration
                           if (C_PuiSvHandler::h_GetInstance()->RemoveViewReadRailItem(this->mu32_ViewIndex,
                                                                                       this->mc_DataElement) ==
                               C_NO_ERR)
                           {
                              //Only restore it if last instance was deleted
                              this->mq_ConfigRestorationNecessary = true;
                           }
                        }
                     }
                  }
               }
            }
         }
         this->mc_DataElement = c_DataElements[0];

         //Add configuration if not already done
         //Only add if new selection is: valid and datapool element
         if ((((this->mc_DataElement.GetIsValid() == true) && (this->mq_NewConfigAdded == false)) &&
              (this->mq_ReadElement == true)) &&
             (this->mc_DataElement.GetType() == C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT))
         {
            const C_OSCNodeDataPoolListElement * const pc_Element =
               C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(this->mc_DataElement.u32_NodeIndex,
                                                                          this->mc_DataElement.u32_DataPoolIndex,
                                                                          this->mc_DataElement.u32_ListIndex,
                                                                          this->mc_DataElement.u32_ElementIndex);
            if ((pc_Element != NULL) && (pc_Element->GetArray() == false))
            {
               C_PuiSvReadDataConfiguration c_Config;
               c_Config.u8_RailIndex = 1;
               if (((pc_Element->GetType() == C_OSCNodeDataPoolContent::eFLOAT64) ||
                    (pc_Element->GetType() == C_OSCNodeDataPoolContent::eSINT64)) ||
                   (pc_Element->GetType() == C_OSCNodeDataPoolContent::eUINT64))
               {
                  c_Config.e_TransmissionMode = C_PuiSvReadDataConfiguration::eTM_ON_TRIGGER;
               }
               else
               {
                  c_Config.e_TransmissionMode = C_PuiSvReadDataConfiguration::eTM_CYCLIC;
               }
               c_Config.InitDefaultThreshold(pc_Element->c_MinValue, pc_Element->c_MaxValue);
               if (C_PuiSvHandler::h_GetInstance()->AddViewReadRailItem(this->mu32_ViewIndex, this->mc_DataElement,
                                                                        c_Config) == C_NO_ERR)
               {
                  //Remember we were the one who added this configuration
                  this->mq_NewConfigAdded = true;
               }
            }
         }

         this->m_InitDataElement(this->mc_DataElement, C_PuiSvDbDataElementScaling());
      }
   }
   //Hide overlay after dialog is not relevant anymore
   if (c_New != NULL)
   {
      pc_Dialog->SaveUserSettings();
      c_New->HideOverlay();
   }
   //lint -e{429}  no memory leak because of the parent of pc_Dialog and the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   Slot of configuration button

   \created     08.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaPeBase::m_Configuration(void) const
{
   QPointer<C_OgePopUpDialog> c_New;
   C_SyvDaPeUpdateModeConfiguration * pc_Dialog;

   stw_types::uint32 u32_LabelIndex = 0;
   const C_PuiSvDbDataElement::E_Type e_Type = C_PuiSvDbDataElement::eLABEL;

   //Before opening the dialog we may need to add the new data element manually so the configuration sees a usage
   // and shows the element, later we have to revert to the original state
   if (this->mq_NewConfigAdded == true)
   {
      C_PuiSvDbLabel c_Label;
      C_PuiSvDbNodeDataElementConfig c_Config;
      c_Config.c_ElementId = this->mc_DataElement;
      c_Label.c_DataPoolElementsConfig.push_back(c_Config);
      if (C_PuiSvHandler::h_GetInstance()->AddDashboardWidget(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                              &c_Label, e_Type) == C_NO_ERR)
      {
         const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
         if (pc_View != NULL)
         {
            const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(this->mu32_DashboardIndex);
            if (pc_Dashboard != NULL)
            {
               tgl_assert(pc_Dashboard->GetLabels().size() > 0UL);
               u32_LabelIndex = pc_Dashboard->GetLabels().size() - 1UL;
            }
         }
      }
      else
      {
         tgl_assert(false);
      }
   }

   //Dialog
   //Set parent for better hierarchy handling via window manager
   c_New = new C_OgePopUpDialog(this->parentWidget(), this->parentWidget());

   pc_Dialog = new C_SyvDaPeUpdateModeConfiguration(*c_New,
                                                    this->mu32_ViewIndex);

   //Initial selection
   pc_Dialog->Select(this->mc_DataElement);

   //Resize
   c_New->SetSize(QSize(1400, 809));

   Q_UNUSED(pc_Dialog)

   if (c_New->exec() == static_cast<sintn>(QDialog::Accepted))
   {
      this->m_InitDataElement(this->mc_DataElement, this->GetScalingInformation());
   }

   //Clean up
   if (this->mq_NewConfigAdded == true)
   {
      tgl_assert(C_PuiSvHandler::h_GetInstance()->DeleteDashboardWidget(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                                        u32_LabelIndex, e_Type) == C_NO_ERR);
   }

   //Hide overlay after dialog is not relevant anymore
   if (c_New != NULL)
   {
      c_New->HideOverlay();
   }

   //lint -e{429}  no memory leak because of the parent of pc_Dialog and the Qt memory management
}

//-----------------------------------------------------------------------------
/*!
   \brief   Register change of use default scaling property

   \created     12.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaPeBase::m_OnUseDefaultScalingChange(void) const
{
   bool q_Enabled;

   if (this->mpc_Ui->pc_CheckBoxDefaultScaling->isChecked() == true)
   {
      q_Enabled = false;
      //Load default values
      if (this->mc_DataElement.GetIsValid() == true)
      {
         const C_OSCNodeDataPoolListElement * const pc_Element =
            C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(this->mc_DataElement.u32_NodeIndex,
                                                                       this->mc_DataElement.u32_DataPoolIndex,
                                                                       this->mc_DataElement.u32_ListIndex,
                                                                       this->mc_DataElement.u32_ElementIndex);
         if (pc_Element != NULL)
         {
            this->mpc_Ui->pc_DoubleSpinBoxOffset->setValue(pc_Element->f64_Offset);
            this->mpc_Ui->pc_DoubleSpinBoxFactor->setValue(pc_Element->f64_Factor);
            this->mpc_Ui->pc_LineEditUnit->setText(pc_Element->c_Unit.c_str());
         }
         else
         {
            this->mpc_Ui->pc_DoubleSpinBoxOffset->setValue(0.0);
            this->mpc_Ui->pc_DoubleSpinBoxFactor->setValue(1.0);
            this->mpc_Ui->pc_LineEditUnit->setText("");
         }
      }
      else
      {
         this->mpc_Ui->pc_DoubleSpinBoxOffset->setValue(0.0);
         this->mpc_Ui->pc_DoubleSpinBoxFactor->setValue(1.0);
         this->mpc_Ui->pc_LineEditUnit->setText("");
      }
   }
   else
   {
      q_Enabled = true;
   }
   this->mpc_Ui->pc_DoubleSpinBoxOffset->setEnabled(q_Enabled);
   this->mpc_Ui->pc_DoubleSpinBoxFactor->setEnabled(q_Enabled);
   this->mpc_Ui->pc_LineEditUnit->setEnabled(q_Enabled);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize no data element state

   \created     06.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaPeBase::m_InitNoDataElement(void) const
{
   this->mpc_Ui->pc_LineEditDataElement->setText("-");
   this->mpc_Ui->pc_LabelUpdateMode->setText("-");

   //Scaling
   this->mpc_Ui->pc_CheckBoxDefaultScaling->setChecked(true);
   this->mpc_Ui->pc_PushButtonUpdateModeConfigure->setVisible(true);
   m_OnUseDefaultScalingChange();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Initialize valid data element state

   \param[in] orc_Id      Element id to use for default initialization (only used if not invalid)
   \param[in] orc_Scaling Scaling information for default initialization

   \created     06.09.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaPeBase::m_InitDataElement(const C_PuiSvDbNodeDataPoolListElementId & orc_Id,
                                      const C_PuiSvDbDataElementScaling & orc_Scaling) const
{
   if (orc_Id.GetIsValid() == true)
   {
      const C_OSCNodeDataPoolListElement * const pc_Element =
         C_PuiSdHandler::h_GetInstance()->GetOSCDataPoolListElement(
            orc_Id.u32_NodeIndex, orc_Id.u32_DataPoolIndex,
            orc_Id.u32_ListIndex,
            orc_Id.u32_ElementIndex);

      if (pc_Element != NULL)
      {
         const QString c_UpdateModeInfo = C_SyvUtil::h_GetUpdateModeDescription(this->mu32_ViewIndex, orc_Id);
         QString c_Edit;
         if (orc_Id.GetType() == C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT)
         {
            c_Edit = C_PuiSdHandler::h_GetInstance()->GetNamespace(orc_Id);
         }
         else
         {
            c_Edit = C_PuiSdHandler::h_GetInstance()->GetSignalNamespace(orc_Id);
         }

         this->mpc_Ui->pc_LineEditDataElement->setText(c_Edit);
         this->mpc_Ui->pc_LineEditDataElement->SetToolTipInformation("", c_Edit);
         this->mpc_Ui->pc_LabelNamespaceValue->setText(c_Edit);
         if (this->mc_DisplayName.compare("") == 0)
         {
            this->mpc_Ui->pc_LineEditDataElementDisplayName->setText(pc_Element->c_Name.c_str());
         }
         else
         {
            this->mpc_Ui->pc_LineEditDataElementDisplayName->setText(this->mc_DisplayName);
         }
         this->mpc_Ui->pc_CheckBoxDefaultScaling->setChecked(orc_Scaling.q_UseDefault);
         this->mpc_Ui->pc_DoubleSpinBoxOffset->setValue(orc_Scaling.f64_Offset);
         this->mpc_Ui->pc_DoubleSpinBoxFactor->setValue(orc_Scaling.f64_Factor);
         this->mpc_Ui->pc_LineEditUnit->setText(orc_Scaling.c_Unit);
         m_OnUseDefaultScalingChange();

         //Update mode
         this->mpc_Ui->pc_LabelUpdateMode->setText(c_UpdateModeInfo);
         this->mpc_Ui->pc_PushButtonUpdateModeConfigure->setVisible(
            orc_Id.GetType() == C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT);
      }
      else
      {
         m_InitNoDataElement();
      }
   }
   else
   {
      m_InitNoDataElement();
   }
}
