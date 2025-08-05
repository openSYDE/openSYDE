//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system view dashboard widget properties (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <climits>
#include <QVBoxLayout>

#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "C_SyvUtil.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_GtGetText.hpp"
#include "C_SyvDaPeBase.hpp"
#include "C_GiSvDaRectBaseGroup.hpp"
#include "C_SyvDaPeUpdateModeConfiguration.hpp"
#include "C_SyvDaPeDataElementBrowse.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "C_SdNdeDpContentUtil.hpp"
#include "ui_C_SyvDaPeBase.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */
const int32_t C_SyvDaPeBase::mhs32_INDEX_MANUAL = 0;
const int32_t C_SyvDaPeBase::mhs32_INDEX_ON_CHANGE = 1;
const int32_t C_SyvDaPeBase::mhs32_INDEX_THEME_OPENSYDE = 0;
const int32_t C_SyvDaPeBase::mhs32_INDEX_THEME_OPENSYDE_2 = 1;
const int32_t C_SyvDaPeBase::mhs32_INDEX_THEME_FLAT = 2;
const int32_t C_SyvDaPeBase::mhs32_INDEX_THEME_SKEUMORPH = 3;
const int32_t C_SyvDaPeBase::mhs32_DASHBOARD_CONNECT_DISABLED = 0;
const int32_t C_SyvDaPeBase::mhs32_DASHBOARD_CONNECT_SETCONSTANTVALUE = 1;
const int32_t C_SyvDaPeBase::mhs32_DASHBOARD_CONNECT_READSERVERVALUE = 2;

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  orc_Parent                   Reference to parent
   \param[in]      ou32_ViewIndex               View index
   \param[in]      ou32_DashboardIndex          Dashboard index
   \param[in]      orc_Name                     Name of the item for the title
   \param[in]      orc_Id                       Element id to use for default initialization (only used if not invalid)
   \param[in]      orc_Scaling                  Scaling information for default initialization
   \param[in]      oq_UseFormatterConfig        Flag if display formatter is available for the user
   \param[in]      orc_FormatterConfig          Formatter information for default initialization
   \param[in]      oq_ReadElement               Optional flag if dialog for read element
   \param[in]      oq_DarkMode                  Optional flag if dark mode active
   \param[in]      oq_ShowWidgetSpecificPart    Optional flag if widget specific part is visible
   \param[in]      oq_AllowChangeOfDataElement  Optional flag if data element may be changed
   \param[in]      orc_DisplayName              Optional display name (only used if element name cannot be changed)
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaPeBase::C_SyvDaPeBase(C_OgePopUpDialog & orc_Parent, const uint32_t ou32_ViewIndex,
                             const uint32_t ou32_DashboardIndex, const QString & orc_Name,
                             const C_PuiSvDbNodeDataPoolListElementId & orc_Id,
                             const C_PuiSvDbDataElementScaling & orc_Scaling, const bool oq_UseFormatterConfig,
                             const C_PuiSvDbDataElementDisplayFormatter & orc_FormatterConfig,
                             const bool oq_ReadElement, const bool oq_DarkMode, const bool oq_ShowWidgetSpecificPart,
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

   // init static names
   this->mpc_ParentDialog->SetTitle(static_cast<QString>(C_GtGetText::h_GetText("%1 Widget")).arg(orc_Name));
   this->InitStaticNames();

   //Factor needs to be above zero
   //lint -e{1938}  static const is guaranteed preinitialized before main
   this->mpc_Ui->pc_DoubleSpinBoxFactor->SetMinimumCustom(C_OgeSpxFactor::mhf64_FACTOR_MIN);

   //Debug texts
   this->mpc_Ui->pc_GroupBoxWrite->setTitle("");
   this->mpc_Ui->pc_GroupBoxRead->setTitle("");
   this->mpc_Ui->pc_GroupBoxInitialValue->setTitle("");
   this->mpc_Ui->pc_PushButtonClearDataElement->setText("");
   this->mpc_Ui->pc_PushButtonDataElement->setText("");

   //Button icon
   this->mpc_Ui->pc_PushButtonClearDataElement->setIcon(QIcon("://images/system_views/IconClearAllEnabled.svg"));
   this->mpc_Ui->pc_PushButtonClearDataElement->setIconSize(QSize(20, 20));
   this->mpc_Ui->pc_PushButtonClearDataElement->setMenu(NULL); // remove menu

   C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LineEditDataElement, "NoRightBorder", true);

   //Type
   if (oq_ReadElement == true)
   {
      this->mpc_Ui->pc_GroupBoxWrite->setVisible(false);
      this->mpc_Ui->pc_GroupBoxInitialValue->setVisible(false);
      this->mpc_Ui->pc_ToggleInitialValue->setVisible(false);
   }
   else
   {
      this->mpc_Ui->pc_GroupBoxRead->setVisible(false);
   }

   // Display formatter
   this->mpc_Ui->pc_WidgetFormatter->setVisible(oq_UseFormatterConfig);

   //Deactivate certain fields
   this->mpc_Ui->pc_LineEditDataElement->setReadOnly(true);

   //Init date element fields
   if (orc_Id.GetIsValid() == true)
   {
      m_InitDataElement(orc_Id, orc_Scaling, orc_FormatterConfig);
   }
   else
   {
      m_InitNoDataElement();
   }

   // create scene for graphics view
   this->mpc_Scene = new C_SyvDaDashboardScene(0, 0, false, NULL);

   // configure background drawing
   this->mpc_Scene->setSceneRect(0.0, 0.0,
                                 static_cast<float64_t>(c_Size.width()),
                                 static_cast<float64_t>(c_Size.height()));
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

   //Spin box
   this->mpc_Ui->pc_DoubleSpinBoxOffset->SetMinimumCustom(std::numeric_limits<float64_t>::lowest());
   this->mpc_Ui->pc_DoubleSpinBoxOffset->SetMaximumCustom(std::numeric_limits<float64_t>::max());

   // connects
   connect(this->mpc_Ui->pc_BushButtonOk, &QPushButton::clicked, this, &C_SyvDaPeBase::m_OkClicked);
   connect(this->mpc_Ui->pc_BushButtonCancel, &QPushButton::clicked,
           this, &C_SyvDaPeBase::m_CancelClicked);
   connect(this->mpc_Ui->pc_PushButtonClearDataElement, &QPushButton::clicked, this, &C_SyvDaPeBase::m_Clear);
   connect(this->mpc_Ui->pc_PushButtonDataElement, &QPushButton::clicked, this, &C_SyvDaPeBase::m_Browse);
   connect(this->mpc_Ui->pc_PushButtonUpdateModeConfigure, &QPushButton::clicked, this,
           &C_SyvDaPeBase::m_Configuration);
   connect(this->mpc_Ui->pc_CheckBoxDefaultScaling, &C_OgeChxProperties::toggled, this,
           &C_SyvDaPeBase::m_OnUseDefaultScalingChange);
   connect(this->mpc_Ui->pc_CheckBoxFormatterActive, &C_OgeChxProperties::toggled, this,
           &C_SyvDaPeBase::m_OnFormatterActiveChange);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxTheme,
           static_cast<void (QComboBox::*)(int32_t)>(&C_OgeCbxText::currentIndexChanged),
           this, &C_SyvDaPeBase::SigRefresh);
   connect(this->mpc_Ui->pc_LineEditFormatterString, &QLineEdit::textChanged, this,
           &C_SyvDaPeBase::m_CheckFormatterString);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_ComboBoxInitialValueMode,
           static_cast<void (QComboBox::*)(int32_t)>(&C_OgeCbxText::currentIndexChanged),
           this, &C_SyvDaPeBase::m_OnInitialValueModeChange);
   connect(this->mpc_Ui->pc_CheckBoxDefaultScaling, &C_OgeChxProperties::toggled,
           this, &C_SyvDaPeBase::m_UpdateSpinboxMetaData);
   //lint -e{929} Cast required to avoid ambiguous signal of qt interface
   connect(this->mpc_Ui->pc_DoubleSpinBoxFactor,
           static_cast<void (QDoubleSpinBox::*)(float64_t)>(&QDoubleSpinBox::valueChanged),
           this, &C_SyvDaPeBase::m_UpdateSpinboxMetaData);
   connect(this->mpc_Ui->pc_DoubleSpinBoxOffset,
           static_cast<void (QDoubleSpinBox::*)(float64_t)>(&QDoubleSpinBox::valueChanged),
           this, &C_SyvDaPeBase::m_UpdateSpinboxMetaData);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because the ownership of these objects was never transferred to this class
C_SyvDaPeBase::~C_SyvDaPeBase(void) noexcept
{
   delete mpc_Ui;
   delete mpc_Scene;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize all displayed static names
*/
//----------------------------------------------------------------------------------------------------------------------
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
   this->mpc_Ui->pc_LabelFormatter->setText(C_GtGetText::h_GetText("Display Formatter"));
   this->mpc_Ui->pc_LabelFormatterString->setText(C_GtGetText::h_GetText("Printf Formatter String"));
   this->mpc_Ui->pc_CheckBoxFormatterActive->setText(C_GtGetText::h_GetText(
                                                        "Enable Display Formatter"));
   this->mpc_Ui->pc_LineEditFormatterString->setPlaceholderText(C_GtGetText::h_GetText("e.g.: \"%.2f\", "
                                                                                       "\"0x%x\", ..."));
   this->mpc_Ui->pc_LabelNamespaceDescription->setText(C_GtGetText::h_GetText("Source"));
   this->mpc_Ui->pc_CheckBoxDefaultScaling->setText(C_GtGetText::h_GetText(
                                                       "Use the default values of SYSTEM DEFINITION"));
   this->mpc_Ui->pc_PushButtonUpdateModeConfigure->setText(C_GtGetText::h_GetText("Configure"));
   this->mpc_Ui->pc_ComboBoxTransmissionMode->addItem(C_GtGetText::h_GetText("On Trigger"));
   this->mpc_Ui->pc_ComboBoxTransmissionMode->addItem(C_GtGetText::h_GetText("On Change"));
   this->mpc_Ui->pc_LabelTheme->setText(C_GtGetText::h_GetText("Theme"));
   this->mpc_Ui->pc_LabelDisplayName->setText(C_GtGetText::h_GetText("Display as"));
   this->mpc_Ui->pc_ComboBoxTheme->addItem(C_GtGetText::h_GetText("openSYDE"));
   this->mpc_Ui->pc_ComboBoxTheme->addItem(C_GtGetText::h_GetText("Material"));
   this->mpc_Ui->pc_ComboBoxTheme->addItem(C_GtGetText::h_GetText("Flat"));
   this->mpc_Ui->pc_ComboBoxTheme->addItem(C_GtGetText::h_GetText("Skeuomorphic"));
   this->mpc_Ui->pc_LabelDashboardConnectHandling->setText(C_GtGetText::h_GetText("Handling on Dashboard Connect"));
   this->mpc_Ui->pc_LabelInitialValueMode->setText(C_GtGetText::h_GetText("Initial Value Mode"));
   this->mpc_Ui->pc_LabelInitialValue->setText(C_GtGetText::h_GetText("Value"));
   this->mpc_Ui->pc_CheckBoxDashboardConnectWrite->setText(C_GtGetText::h_GetText(
                                                              "Auto write value on dashboard connect"));
   this->mpc_Ui->pc_ComboBoxInitialValueMode->addItem(C_GtGetText::h_GetText("Disabled"));
   this->mpc_Ui->pc_ComboBoxInitialValueMode->addItem(C_GtGetText::h_GetText("Set Constant Value"));
   this->mpc_Ui->pc_ComboBoxInitialValueMode->addItem(C_GtGetText::h_GetText("Read Server Value"));

   //Tool tips
   this->mpc_Ui->pc_LabelTheme->SetToolTipInformation(
      C_GtGetText::h_GetText("Theme"),
      C_GtGetText::h_GetText("General display style of the widget grouped into different common themes"));
   this->mpc_Ui->pc_LabelDataElement->SetToolTipInformation(
      C_GtGetText::h_GetText("Data Element"),
      C_GtGetText::h_GetText("The currently selected data element defined in the SYSTEM DEFINITION"));
   this->mpc_Ui->pc_LabelUpdateModeHeading->SetToolTipInformation(
      C_GtGetText::h_GetText("Update Mode"),
      C_GtGetText::h_GetText("The rate at which the data element value is requested,"
                             " once the dashboard is connected"));
   this->mpc_Ui->pc_LabelFormatterString->SetToolTipInformation(
      C_GtGetText::h_GetText("Printf Formatter String"),
      C_GtGetText::h_GetText("Configuration of the Display Formatter. The printf syntax can be used."));
   this->mpc_Ui->pc_CheckBoxDefaultScaling->SetToolTipInformation(
      C_GtGetText::h_GetText("Use the default values of SYSTEM DEFINITION"),
      C_GtGetText::h_GetText("Option to either use the scaling defined in the SYSTEM DEFINITION\n"
                             "or define a custom scaling just for this occurrence of this data element"));
   this->mpc_Ui->pc_CheckBoxFormatterActive->SetToolTipInformation(
      C_GtGetText::h_GetText("Activate the Display Formatter"),
      C_GtGetText::h_GetText("Option to either use the formatting style defined in the printf formatter string below\n"
                             "or use the standard formatting of the Dashboard"));
   this->mpc_Ui->pc_PushButtonClearDataElement->SetToolTipInformation(
      C_GtGetText::h_GetText("Clear Data Element"),
      C_GtGetText::h_GetText("Remove currently selected data element."));
   this->mpc_Ui->pc_PushButtonDataElement->SetToolTipInformation(
      C_GtGetText::h_GetText("Browse Data Element"),
      C_GtGetText::h_GetText("Select a data element from list of all data elements."));
   this->mpc_Ui->pc_PushButtonUpdateModeConfigure->SetToolTipInformation(
      C_GtGetText::h_GetText("Configure Update Mode"),
      C_GtGetText::h_GetText("Configure transmission mode, cycle intervals and change threshold."));

   //Tool tip content partly adapted from Vector CANdb++ Documentation
   {
      const QString c_PhysicalValueInfo =
         C_GtGetText::h_GetText("The raw value of a data element is the value as it is transmitted in the network."
                                "\nThe physical value of a data element is the value of the physical quantity (e.g. speed, "
                                "\nrpm, temperature, etc.) that represents the data element."
                                "\nThe following conversion formula is used to transform the raw value "
                                "\nto a physical value or in the reverse direction:"
                                "\n\n[Physical value] = ([Raw value] * [Factor]) + [Offset]");
      QString c_InfoText =  C_GtGetText::h_GetText("Unit of the data elements physical quantity\n\n");
      this->mpc_Ui->pc_LabelUnit->SetToolTipInformation(C_GtGetText::h_GetText("Unit"),
                                                        c_InfoText.append(c_PhysicalValueInfo));
      this->mpc_Ui->pc_LabelFactor->SetToolTipInformation(C_GtGetText::h_GetText("Factor"),
                                                          c_PhysicalValueInfo);
      this->mpc_Ui->pc_LabelOffset->SetToolTipInformation(C_GtGetText::h_GetText("Offset"),
                                                          c_PhysicalValueInfo);
   }

   this->mpc_Ui->pc_LabelInitialValueMode->SetToolTipInformation(
      C_GtGetText::h_GetText("Initial Value Mode"),
      C_GtGetText::h_GetText("Options for handling of initial value on dashboard connect:\n\n"
                             "Disabled: Use the value that was set before connecting.\n"
                             "Set constant value: Set the value to a constant before connecting.\n"
                             "Read server value: Read the current value from the server and set the widgets "
                             "value to it before connecting.\n"));
   this->mpc_Ui->pc_CheckBoxDashboardConnectWrite->SetToolTipInformation(
      C_GtGetText::h_GetText("Auto write value on dashboard connect"),
      C_GtGetText::h_GetText(
         "Option: Set value should be automatically sent once when connecting to the dashboard."));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Place widget in pop up dialog

   \param[in,out]  opc_Widget    Widget to place in pop up dialog
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeBase::SetWidget(QWidget * const opc_Widget)
{
   if (opc_Widget != NULL)
   {
      opc_Widget->setParent(this);
      this->mpc_Ui->pc_VerticalLayoutDesign->insertWidget(1, opc_Widget, 1);
   } //lint !e429  //no memory leak because of the parent of pc_Layout and the Qt memory management
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the write mode

   \param[in]  oe_WriteMode   New write mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeBase::SetWriteMode(const C_PuiSvDbWriteWidgetBase::E_WriteMode oe_WriteMode) const
{
   if (this->mc_DataElement.GetIsValid() == true)
   {
      switch (oe_WriteMode)
      {
      case C_PuiSvDbWriteWidgetBase::eWM_MANUAL:
         this->mpc_Ui->pc_ComboBoxTransmissionMode->setCurrentIndex(C_SyvDaPeBase::mhs32_INDEX_MANUAL);
         break;
      case C_PuiSvDbWriteWidgetBase::eWM_ON_CHANGE:
         this->mpc_Ui->pc_ComboBoxTransmissionMode->setCurrentIndex(C_SyvDaPeBase::mhs32_INDEX_ON_CHANGE);
         break;
      default:
         this->mpc_Ui->pc_ComboBoxTransmissionMode->setCurrentIndex(C_SyvDaPeBase::mhs32_INDEX_MANUAL);
         break;
      }
   }
   else
   {
      this->mpc_Ui->pc_ComboBoxTransmissionMode->setCurrentIndex(C_SyvDaPeBase::mhs32_INDEX_MANUAL);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set theme

   \param[in]  oe_Style    Current theme
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeBase::SetTheme(const C_PuiSvDbWidgetBase::E_Style oe_Style) const
{
   switch (oe_Style)
   {
   case C_PuiSvDbWidgetBase::eOPENSYDE:
      this->mpc_Ui->pc_ComboBoxTheme->setCurrentIndex(C_SyvDaPeBase::mhs32_INDEX_THEME_OPENSYDE);
      break;
   case C_PuiSvDbWidgetBase::eOPENSYDE_2:
      this->mpc_Ui->pc_ComboBoxTheme->setCurrentIndex(C_SyvDaPeBase::mhs32_INDEX_THEME_OPENSYDE_2);
      break;
   case C_PuiSvDbWidgetBase::eSKEUOMORPH:
      this->mpc_Ui->pc_ComboBoxTheme->setCurrentIndex(C_SyvDaPeBase::mhs32_INDEX_THEME_SKEUMORPH);
      break;
   case C_PuiSvDbWidgetBase::eFLAT:
      this->mpc_Ui->pc_ComboBoxTheme->setCurrentIndex(C_SyvDaPeBase::mhs32_INDEX_THEME_FLAT);
      break;
   default:
      break;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the preview scene

   \return     Pointer to preview scene
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaDashboardScene * C_SyvDaPeBase::GetPreviewScene(void)
{
   return this->mpc_Scene;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get scene view size for improved item placement

   \return
   Scene view size
*/
//----------------------------------------------------------------------------------------------------------------------
QSize C_SyvDaPeBase::h_GetSceneViewSize(void)
{
   return QSize(259, 160);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get offset for icon placement

   \return
   Offset for icon placement
*/
//----------------------------------------------------------------------------------------------------------------------
float64_t C_SyvDaPeBase::h_GetFixIconOffset(void)
{
   return 25.0;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get ideal item size according to current scene size

   \param[out]  orc_ItemPos   Ideal item position
   \param[out]  orc_ItemSize  Ideal item size
   \param[in]   opc_Item      Graphics item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeBase::h_GetIdealItemRect(QPointF & orc_ItemPos, QSizeF & orc_ItemSize,
                                       const QGraphicsItem * const opc_Item)
{
   const C_GiSvDaRectBaseGroup * const pc_GiItem = dynamic_cast<const C_GiSvDaRectBaseGroup * const>(opc_Item);

   if (pc_GiItem != NULL)
   {
      const QSize c_ViewSize = C_SyvDaPeBase::h_GetSceneViewSize();
      //To be really centered we also have to include the borders into the size
      const float64_t f64_DoubleInteractionPointOffset =
         (pc_GiItem->boundingRect().width() - pc_GiItem->GetVisibleBoundingRect().width());
      //Also include the fix offset to the right
      const float64_t f64_IconOffset = C_SyvDaPeBase::h_GetFixIconOffset();
      orc_ItemSize = QSizeF(
         ((static_cast<float64_t>(c_ViewSize.width()) / 1.5) + f64_IconOffset) + f64_DoubleInteractionPointOffset,
         (static_cast<float64_t>(c_ViewSize.height()) / 1.5) + f64_DoubleInteractionPointOffset);
      orc_ItemPos = QPointF(
         ((static_cast<float64_t>(c_ViewSize.width()) - orc_ItemSize.width()) / 2.0) + (f64_IconOffset / 2.0),
         (static_cast<float64_t>(c_ViewSize.height()) - orc_ItemSize.height()) / 2.0);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current data element id

   \return
   Current data element id
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbNodeDataPoolListElementId C_SyvDaPeBase::GetDataElementId(void) const
{
   return this->mc_DataElement;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current scaling information

   \return
   Current scaling information
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbDataElementScaling C_SyvDaPeBase::GetScalingInformation() const
{
   C_PuiSvDbDataElementScaling c_Retval;

   c_Retval.q_UseDefault = this->mpc_Ui->pc_CheckBoxDefaultScaling->isChecked();
   c_Retval.f64_Offset = this->mpc_Ui->pc_DoubleSpinBoxOffset->value();
   c_Retval.f64_Factor = this->mpc_Ui->pc_DoubleSpinBoxFactor->value();
   c_Retval.c_Unit = this->mpc_Ui->pc_LineEditUnit->text();
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current formatter information

   \return
   Current formatter information
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbDataElementDisplayFormatter C_SyvDaPeBase::GetFormatterInformation(void) const
{
   C_PuiSvDbDataElementDisplayFormatter c_Retval;

   c_Retval.q_IsActive = this->mpc_Ui->pc_CheckBoxFormatterActive->isChecked();
   c_Retval.c_FormatterString = this->mpc_Ui->pc_LineEditFormatterString->text();

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current write mode

   \return
   Current write mode
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbWriteWidgetBase::E_WriteMode C_SyvDaPeBase::GetWriteMode(void) const
{
   C_PuiSvDbWriteWidgetBase::E_WriteMode e_Retval;
   switch (this->mpc_Ui->pc_ComboBoxTransmissionMode->currentIndex())
   {
   case C_SyvDaPeBase::mhs32_INDEX_MANUAL:
      e_Retval = C_PuiSvDbWriteWidgetBase::eWM_MANUAL;
      break;
   case C_SyvDaPeBase::mhs32_INDEX_ON_CHANGE:
      e_Retval = C_PuiSvDbWriteWidgetBase::eWM_ON_CHANGE;
      break;
   default:
      e_Retval = C_PuiSvDbWriteWidgetBase::eWM_MANUAL;
      break;
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current theme

   \return
   Current theme
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbWidgetBase::E_Style C_SyvDaPeBase::GetTheme(void) const
{
   C_PuiSvDbWidgetBase::E_Style e_Retval;
   switch (this->mpc_Ui->pc_ComboBoxTheme->currentIndex())
   {
   case C_SyvDaPeBase::mhs32_INDEX_THEME_OPENSYDE:
      e_Retval = C_PuiSvDbWidgetBase::eOPENSYDE;
      break;
   case C_SyvDaPeBase::mhs32_INDEX_THEME_OPENSYDE_2:
      e_Retval = C_PuiSvDbWidgetBase::eOPENSYDE_2;
      break;
   case C_SyvDaPeBase::mhs32_INDEX_THEME_FLAT:
      e_Retval = C_PuiSvDbWidgetBase::eFLAT;
      break;
   case C_SyvDaPeBase::mhs32_INDEX_THEME_SKEUMORPH:
      e_Retval = C_PuiSvDbWidgetBase::eSKEUOMORPH;
      break;
   default:
      e_Retval = C_PuiSvDbWidgetBase::eOPENSYDE;
      break;
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current display name

   \return
   Current display name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDaPeBase::GetDisplayName(void) const
{
   QString c_Retval = this->mpc_Ui->pc_LineEditDataElementDisplayName->text();

   if (this->mc_DataElement.GetIsValid() == true)
   {
      if (c_Retval.compare(this->m_GetDefaultDisplayName(this->mc_DataElement)) == 0)
      {
         c_Retval = "";
      }
   }

   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get popup size with display formatter
 *
 *  Provides the size for a popup for widget properties that
 *  contains the display formatter
 *
 *  \return
 *   QSize
*/
QSize C_SyvDaPeBase::h_GetPopupSizeWithDisplayFormatter(void)
{
   const QSize c_SIZE(800, 967);

   return c_SIZE;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get popup size with NO display formatter
 *
 *   Provides the size for a popup for widget properties that
 *   does not contain the display formatter
 *
 *   \return
 *    QSize
*/
QSize C_SyvDaPeBase::h_GetPopupSizeWithoutDisplayFormatter(void)
{
   const QSize c_SIZE(800, 800);

   return c_SIZE;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get popup size with NO design and preview panes
 *
 *   Provides the size for a popup for widget properties that does not contain
 *   the design and preview panes but may contain the display formatter
 *   e.g. Table widget - Edit Properites Dialog (C_GiSvDaTableBase::EditElementProperties)
 *
 *   \return
 *    QSize
*/
QSize C_SyvDaPeBase::h_GetPopupSizeWithoutDesignAndPreview()
{
   const QSize c_SIZE(800, 723);

   return c_SIZE;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get slider popup size with dashboard connect
 *
 *   Provides the size for a slider popup widget with dashboard connect
 *
 *   \return
 *    QSize
*/
//----------------------------------------------------------------------------------------------------------------------
QSize C_SyvDaPeBase::h_GetSliderPopupSizeWithDashboardConnect()
{
   const QSize c_SIZE(800, 1060);

   return c_SIZE;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get spinbox popup size with dashboard connect
 *
 *   Provides the size for a spinbox popup widget with dashboard connect
 *
 *   \return
 *    QSize
*/
//----------------------------------------------------------------------------------------------------------------------
QSize C_SyvDaPeBase::h_GetSpinBoxPopupSizeWithDashboardConnect()
{
   const QSize c_SIZE(800, 900);

   return c_SIZE;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get toggle popup size with dashboard connect
 *
 *   Provides the size for a toggle popup widget with dashboard connect
 *
 *   \return
 *    QSize
*/
//----------------------------------------------------------------------------------------------------------------------
QSize C_SyvDaPeBase::h_GetTogglePopupSizeWithDashboardConnect()
{
   const QSize c_SIZE(800, 850);

   return c_SIZE;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dashboard connect initial value mode and value

   \param[in]  oe_DashboardConnectMode    New dashboard connect mode
   \param[in]  orc_InitialValue           Initial value
   \param[in]  oq_IsToggle                Flag to decide wether to show toggle or spinbox for initial value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeBase::SetDashboardConnectInitialValue(
   const C_PuiSvDbWriteWidgetBase::E_InitialValueModeType oe_DashboardConnectMode,
   const C_OscNodeDataPoolContent & orc_InitialValue, const bool oq_IsToggle)
{
   if (this->mc_DataElement.GetIsValid() == true)
   {
      switch (oe_DashboardConnectMode)
      {
      case C_PuiSvDbWriteWidgetBase::eIVM_READ_SERVER_VALUE:
         this->mpc_Ui->pc_ComboBoxInitialValueMode->setCurrentIndex(
            C_SyvDaPeBase::mhs32_DASHBOARD_CONNECT_READSERVERVALUE);
         break;
      case C_PuiSvDbWriteWidgetBase::eIVM_SET_CONSTANT_VALUE:
         this->mpc_Ui->pc_ComboBoxInitialValueMode->setCurrentIndex(
            C_SyvDaPeBase::mhs32_DASHBOARD_CONNECT_SETCONSTANTVALUE);
         break;
      case C_PuiSvDbWriteWidgetBase::eIVM_DISABLED:
         this->mpc_Ui->pc_ComboBoxInitialValueMode->setCurrentIndex(
            C_SyvDaPeBase::mhs32_DASHBOARD_CONNECT_DISABLED);
         break;
      default:
         this->mpc_Ui->pc_ComboBoxInitialValueMode->setCurrentIndex(C_SyvDaPeBase::mhs32_DASHBOARD_CONNECT_DISABLED);
         break;
      }

      this->m_SetDashboardConnectInitialValue(orc_InitialValue);
   }
   else
   {
      this->mpc_Ui->pc_ComboBoxInitialValueMode->setCurrentIndex(C_SyvDaPeBase::mhs32_DASHBOARD_CONNECT_DISABLED);
      // no value initialization as the spinbox/toggle is invisible if "Disabled" is selected
   }

   // always set visibility of toggle and spinbox
   this->mpc_Ui->pc_ToggleInitialValue->setVisible(oq_IsToggle);
   this->mpc_Ui->pc_SpinBoxInitialValue->setVisible(!oq_IsToggle);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current dashboard connect mode

   \return
   Current dashboard connect mode
*/
//----------------------------------------------------------------------------------------------------------------------
C_PuiSvDbWriteWidgetBase::E_InitialValueModeType C_SyvDaPeBase::GetDashboardConnectInitialValueMode(void) const
{
   C_PuiSvDbWriteWidgetBase::E_InitialValueModeType e_Retval;
   switch (this->mpc_Ui->pc_ComboBoxInitialValueMode->currentIndex())
   {
   case C_SyvDaPeBase::mhs32_DASHBOARD_CONNECT_READSERVERVALUE:
      e_Retval = C_PuiSvDbWriteWidgetBase::eIVM_READ_SERVER_VALUE;
      break;
   case C_SyvDaPeBase::mhs32_DASHBOARD_CONNECT_SETCONSTANTVALUE:
      e_Retval = C_PuiSvDbWriteWidgetBase::eIVM_SET_CONSTANT_VALUE;
      break;
   default:
      e_Retval = C_PuiSvDbWriteWidgetBase::eIVM_DISABLED;
      break;
   }
   return e_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get current dashboard connect initial value

   \return
   Current dashboard connect initial value
*/
//----------------------------------------------------------------------------------------------------------------------
C_OscNodeDataPoolContent C_SyvDaPeBase::GetDashboardConnectInitialValue(void) const
{
   C_OscNodeDataPoolContent c_Retval;

   const C_OscNodeDataPoolListElement * const pc_Element =
      C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(this->mc_DataElement.u32_NodeIndex,
                                                                 this->mc_DataElement.u32_DataPoolIndex,
                                                                 this->mc_DataElement.u32_ListIndex,
                                                                 this->mc_DataElement.u32_ElementIndex);

   if (pc_Element != NULL)
   {
      const C_PuiSvDbDataElementScaling c_Scaling = this->GetScalingInformation();

      // set correct data type
      c_Retval.SetType(pc_Element->GetType());
      c_Retval.SetArray(false); // no array support in write widgets

      // differ between spinbox (for spinbox and slider widget) and toggle (for toggle widget)
      if (this->mpc_Ui->pc_ToggleInitialValue->isVisibleTo(this->mpc_Ui->pc_WidgetConstValue) == true)
      {
         float64_t f64_Value = static_cast<float64_t>(this->mpc_Ui->pc_ToggleInitialValue->isChecked());

         if (C_OscUtils::h_IsScalingActive(c_Scaling.f64_Factor, c_Scaling.f64_Offset) == true)
         {
            // Scaling necessary to prevent a rounding error to get 0 instead of 1
            f64_Value = C_OscUtils::h_GetValueScaled(f64_Value, c_Scaling.f64_Factor, c_Scaling.f64_Offset);
         }
         C_SdNdeDpContentUtil::h_SetScaledValueInContent(f64_Value, c_Retval,
                                                         c_Scaling.f64_Factor, c_Scaling.f64_Offset);
      }
      else
      {
         C_SdNdeDpContentUtil::h_SetDataVariableFromGenericWithScaling(
            this->mpc_Ui->pc_SpinBoxInitialValue->GetValue(), c_Retval,
            c_Scaling.f64_Factor, c_Scaling.f64_Offset, 0UL);
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set auto write on connect state

   \param[in]  oq_Value    New state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeBase::SetAutoWriteOnConnect(const bool oq_Value)
{
   this->mpc_Ui->pc_CheckBoxDashboardConnectWrite->setChecked(oq_Value);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get auto write on connect state

   \return
   Get auto write on connect state
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaPeBase::GetAutoWriteOnConnect() const
{
   return this->mpc_Ui->pc_CheckBoxDashboardConnectWrite->isChecked();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten key press event slot

   Here: Handle specific enter key cases

   \param[in,out]  opc_KeyEvent  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeBase::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   bool q_CallOrg = true;

   //Handle all enter key cases manually
   if ((opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Enter)) ||
       (opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_Return)))
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Ok button click
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeBase::m_OkClicked(void)
{
   bool q_Continue = true;
   const C_PuiSvDbDataElementDisplayFormatter c_FormatterConfig = this->GetFormatterInformation();

   // Check formatter first
   if ((c_FormatterConfig.q_IsActive == true) &&
       (this->mc_DataElement.GetIsValid() == true))
   {
      const C_PuiSvDbDataElementScaling c_Scaling = this->GetScalingInformation();
      const C_OscNodeDataPoolListElement * const pc_Element =
         C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(this->mc_DataElement.u32_NodeIndex,
                                                                    this->mc_DataElement.u32_DataPoolIndex,
                                                                    this->mc_DataElement.u32_ListIndex,
                                                                    this->mc_DataElement.u32_ElementIndex);

      tgl_assert(pc_Element != NULL);
      if (pc_Element != NULL)
      {
         if (c_FormatterConfig.IsFormatterCompatible(
                C_PuiSvDbDataElementDisplayFormatter::h_GetTypeCategory(pc_Element->c_MinValue,
                                                                        c_Scaling,
                                                                        pc_Element->q_InterpretAsString)) == false)
         {
            // Error
            C_OgeWiCustomMessage c_MessageBox(this, C_OgeWiCustomMessage::eERROR);
            c_MessageBox.SetHeading(C_GtGetText::h_GetText("Display Formatter"));
            c_MessageBox.SetDescription(
               C_GtGetText::h_GetText(
                  "The configured printf formatter string is invalid or not compatible to the selected data element. \n"
                  "Check user documentation for more information about supported printf parameters.\n\n"
                  "Adapt the printf formatter string or disable the display formatter option."));
            c_MessageBox.SetCustomMinHeight(250, 250);
            c_MessageBox.SetCustomMinWidth(730);
            c_MessageBox.Execute();

            q_Continue = false;
         }
      }
   }

   if ((this->mpc_ParentDialog != NULL) &&
       (q_Continue == true))
   {
      this->mpc_ParentDialog->accept();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of Cancel button
*/
//----------------------------------------------------------------------------------------------------------------------
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of browse button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeBase::m_Browse(void)
{
   //Set parent for better hierarchy handling via window manager
   const QPointer<C_OgePopUpDialog> c_New = new C_OgePopUpDialog(this, this);
   C_SyvDaPeDataElementBrowse * const pc_Dialog = new C_SyvDaPeDataElementBrowse(*c_New, this->mu32_ViewIndex, false,
                                                                                 !this->mq_ReadElement, false,
                                                                                 this->mq_ReadElement, true,
                                                                                 false);

   //Resize
   c_New->SetSize(QSize(800, 800));

   if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      const std::vector<C_PuiSvDbNodeDataPoolListElementId> c_DataElements = pc_Dialog->GetSelectedDataElements();
      //Only accept valid selection (assumed if any elements
      if (c_DataElements.size() > 0)
      {
         m_ClearDataElement();
         this->mc_DataElement = c_DataElements[0];

         //Add configuration if not already done
         //Only add if new selection is: valid and datapool element
         if ((((this->mc_DataElement.GetIsValid() == true) && (this->mq_NewConfigAdded == false)) &&
              (this->mq_ReadElement == true)) &&
             (this->mc_DataElement.GetType() == C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT))
         {
            const C_OscNodeDataPoolListElement * const pc_Element =
               C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(this->mc_DataElement.u32_NodeIndex,
                                                                          this->mc_DataElement.u32_DataPoolIndex,
                                                                          this->mc_DataElement.u32_ListIndex,
                                                                          this->mc_DataElement.u32_ElementIndex);
            if (pc_Element != NULL)
            {
               if (pc_Element->GetArray() == false)
               {
                  C_PuiSvReadDataConfiguration c_Config;
                  c_Config.u8_RailIndex = 1;
                  if (((pc_Element->GetType() == C_OscNodeDataPoolContent::eFLOAT64) ||
                       (pc_Element->GetType() == C_OscNodeDataPoolContent::eSINT64)) ||
                      (pc_Element->GetType() == C_OscNodeDataPoolContent::eUINT64))
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
               else
               {
                  //Array item
                  C_PuiSvReadDataConfiguration c_Config;
                  c_Config.u8_RailIndex = 1;
                  c_Config.e_TransmissionMode = C_PuiSvReadDataConfiguration::eTM_ON_TRIGGER;
                  c_Config.InitDefaultThreshold(pc_Element->c_MinValue, pc_Element->c_MaxValue);
                  if (C_PuiSvHandler::h_GetInstance()->AddViewReadRailItem(this->mu32_ViewIndex, this->mc_DataElement,
                                                                           c_Config) == C_NO_ERR)
                  {
                     //Remember we were the one who added this configuration
                     this->mq_NewConfigAdded = true;
                  }
               }
            }
         }

         this->m_InitDataElement(this->mc_DataElement,
                                 C_PuiSvDbDataElementScaling(), C_PuiSvDbDataElementDisplayFormatter());
      }
   }
   //Hide overlay after dialog is not relevant anymore
   if (c_New != NULL)
   {
      pc_Dialog->SaveUserSettings();
      pc_Dialog->PrepareCleanUp();
      c_New->HideOverlay();
      c_New->deleteLater();
   }
} //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle clear data element button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeBase::m_Clear(void)
{
   m_ClearDataElement();
   this->mc_DataElement.MarkInvalid(stw::opensyde_core::C_OscNodeDataPool::eDIAG, "");
   m_InitNoDataElement();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle clear data element request
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeBase::m_ClearDataElement()
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
               const QMap<C_OscNodeDataPoolListElementId,
                          C_PuiSvReadDataConfiguration>::const_iterator c_ItResult =
                  pc_View->GetReadRailAssignments().find(this->mc_OriginalConfigId);
               tgl_assert(c_ItResult != pc_View->GetReadRailAssignments().end());
               if (c_ItResult != pc_View->GetReadRailAssignments().end())
               {
                  this->mc_OriginalConfigData = c_ItResult.value();
                  const uint32_t u32_Usages = pc_View->CountReadUsage(this->mc_DataElement);
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
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot of configuration button
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeBase::m_Configuration(void) const
{
   QPointer<C_OgePopUpDialog> c_New;
   C_SyvDaPeUpdateModeConfiguration * pc_Dialog;

   uint32_t u32_LabelIndex = 0;
   const C_PuiSvDbDataElement::E_Type e_TYPE = C_PuiSvDbDataElement::eLABEL;

   //Before opening the dialog we may need to add the new data element manually so the configuration sees a usage
   // and shows the element, later we have to revert to the original state
   if (this->mq_NewConfigAdded == true)
   {
      C_PuiSvDbLabel c_Label;
      C_PuiSvDbNodeDataElementConfig c_Config;
      c_Config.c_ElementId = this->mc_DataElement;
      c_Label.c_DataPoolElementsConfig.push_back(c_Config);
      if (C_PuiSvHandler::h_GetInstance()->AddDashboardWidget(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                              &c_Label, e_TYPE) == C_NO_ERR)
      {
         const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);
         if (pc_View != NULL)
         {
            const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(this->mu32_DashboardIndex);
            if (pc_Dashboard != NULL)
            {
               tgl_assert(pc_Dashboard->GetLabels().size() > 0UL);
               u32_LabelIndex = static_cast<uint32_t>(pc_Dashboard->GetLabels().size()) - 1UL;
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

   if (c_New->exec() == static_cast<int32_t>(QDialog::Accepted))
   {
      this->m_InitDataElement(this->mc_DataElement, this->GetScalingInformation(), this->GetFormatterInformation());
   }

   //Clean up
   if (this->mq_NewConfigAdded == true)
   {
      tgl_assert(C_PuiSvHandler::h_GetInstance()->DeleteDashboardWidget(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                                        u32_LabelIndex, e_TYPE) == C_NO_ERR);
   }

   //Hide overlay after dialog is not relevant anymore
   if (c_New != NULL)
   {
      c_New->HideOverlay();
      c_New->deleteLater();
   }
} //lint !e429  //no memory leak because of the parent of pc_Dialog and the Qt memory management

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Get default display name

   \param[in]  orc_Id   ID to use for default

   \return
   Default display name
*/
//----------------------------------------------------------------------------------------------------------------------
QString C_SyvDaPeBase::m_GetDefaultDisplayName(const C_PuiSvDbNodeDataPoolListElementId & orc_Id) const
{
   QString c_Retval;

   if (orc_Id.GetIsValid() == true)
   {
      const C_OscNodeDataPoolListElement * const pc_Element =
         C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(this->mc_DataElement.u32_NodeIndex,
                                                                    this->mc_DataElement.u32_DataPoolIndex,
                                                                    this->mc_DataElement.u32_ListIndex,
                                                                    this->mc_DataElement.u32_ElementIndex);
      const C_OscNodeDataPool * const pc_Datapool =
         C_PuiSdHandler::h_GetInstance()->GetOscDataPool(orc_Id.u32_NodeIndex,
                                                         orc_Id.u32_DataPoolIndex);
      if (pc_Element != NULL)
      {
         if (pc_Datapool != NULL)
         {
            if ((pc_Datapool->e_Type == C_OscNodeDataPool::eHALC) ||
                (pc_Datapool->e_Type == C_OscNodeDataPool::eHALC_NVM))
            {
               c_Retval = C_PuiSvHandler::h_GetShortNamespace(orc_Id);
            }
            else
            {
               const QString c_ElementName = pc_Element->c_Name.c_str();
               if (orc_Id.GetUseArrayElementIndex())
               {
                  c_Retval = static_cast<QString>("%1[%2]").arg(c_ElementName).arg(orc_Id.GetArrayElementIndex());
               }
               else
               {
                  c_Retval = c_ElementName;
               }
            }
         }
      }
   }
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register change of use default scaling property
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeBase::m_OnUseDefaultScalingChange(void) const
{
   bool q_Enabled;

   if (this->mpc_Ui->pc_CheckBoxDefaultScaling->isChecked() == true)
   {
      q_Enabled = false;
      //Load default values
      if (this->mc_DataElement.GetIsValid() == true)
      {
         const C_OscNodeDataPoolListElement * const pc_Element =
            C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(this->mc_DataElement.u32_NodeIndex,
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

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Register change of active display formatter property
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeBase::m_OnFormatterActiveChange(void) const
{
   const bool q_Active = this->mpc_Ui->pc_CheckBoxFormatterActive->isChecked();

   this->mpc_Ui->pc_LineEditFormatterString->setEnabled(q_Active);

   // if display formatter active and contains some text, apply "red style" to text and display red tooltip
   if (q_Active && (!mpc_Ui->pc_LineEditFormatterString->text().isEmpty()))
   {
      m_CheckFormatterString();
   }
   else
   {
      C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LineEditFormatterString, "Valid",
                                             true);
      this->mpc_Ui->pc_LineEditFormatterString->SetToolTipInformation(C_GtGetText::h_GetText(""),
                                                                      C_GtGetText::h_GetText(""),
                                                                      stw::opensyde_gui::C_NagToolTip::eDEFAULT);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check the formatter string

   Checks if the printf string has a valid and compatible format.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeBase::m_CheckFormatterString(void) const
{
   const C_PuiSvDbDataElementDisplayFormatter c_FormatterConfig = this->GetFormatterInformation();

   // Check formatter first
   if ((c_FormatterConfig.q_IsActive == true) &&
       (this->mc_DataElement.GetIsValid() == true))
   {
      const C_PuiSvDbDataElementScaling c_Scaling = this->GetScalingInformation();
      const C_OscNodeDataPoolListElement * const pc_Element =
         C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(this->mc_DataElement.u32_NodeIndex,
                                                                    this->mc_DataElement.u32_DataPoolIndex,
                                                                    this->mc_DataElement.u32_ListIndex,
                                                                    this->mc_DataElement.u32_ElementIndex);

      tgl_assert(pc_Element != NULL);

      // Check if the format is valid and compatible. If not, font color is changed to red and a tool tip is
      // displayed to indicate the error
      if (pc_Element != NULL)
      {
         const bool q_FormatterCompatible = c_FormatterConfig.IsFormatterCompatible(
            C_PuiSvDbDataElementDisplayFormatter::h_GetTypeCategory(pc_Element->c_MinValue,
                                                                    c_Scaling,
                                                                    pc_Element->q_InterpretAsString));

         //set text property
         C_OgeWiUtil::h_ApplyStylesheetProperty(this->mpc_Ui->pc_LineEditFormatterString, "Valid",
                                                q_FormatterCompatible);
         if (!q_FormatterCompatible)
         {
            const QString c_Heading = C_GtGetText::h_GetText("Formatter String");
            QString c_Content;

            c_Content += C_GtGetText::h_GetText("- invalid or not compatible format\n");

            this->mpc_Ui->pc_LineEditFormatterString->SetToolTipInformation(c_Heading, c_Content,
                                                                            stw::opensyde_gui::C_NagToolTip::eERROR);
         }
         else
         {
            this->mpc_Ui->pc_LineEditFormatterString->SetToolTipInformation(C_GtGetText::h_GetText(""),
                                                                            C_GtGetText::h_GetText(""),
                                                                            stw::opensyde_gui::C_NagToolTip::eDEFAULT);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set dashboard connect initial value

   \param[in]  orc_InitialValue  Initial value
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeBase::m_SetDashboardConnectInitialValue(const C_OscNodeDataPoolContent & orc_InitialValue) const
{
   const C_PuiSvDbDataElementScaling c_Scaling = this->GetScalingInformation();
   float64_t f64_UnscaledValue;

   this->mpc_Ui->pc_SpinBoxInitialValue->SetValue(
      C_SdNdeDpContentUtil::h_ConvertScaledContentToGeneric(orc_InitialValue, c_Scaling.f64_Factor,
                                                            c_Scaling.f64_Offset,
                                                            0UL /*no array support in write widgets*/));

   C_SdNdeDpContentUtil::h_GetValueAsFloat64(orc_InitialValue, f64_UnscaledValue, 0UL);
   this->mpc_Ui->pc_ToggleInitialValue->setChecked(static_cast<bool>(f64_UnscaledValue));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Slot for actions when changing initial value mode

   \param[in]  os32_Index  Combobox index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeBase::m_OnInitialValueModeChange(const int32_t os32_Index) const
{
   // spinbox visibility
   if (os32_Index == mhs32_DASHBOARD_CONNECT_SETCONSTANTVALUE)
   {
      this->mpc_Ui->pc_WidgetConstValue->setVisible(true);
   }
   else
   {
      this->mpc_Ui->pc_WidgetConstValue->setVisible(false);
   }

   // checkbox enabeling
   if (os32_Index == mhs32_DASHBOARD_CONNECT_READSERVERVALUE)
   {
      this->mpc_Ui->pc_CheckBoxDashboardConnectWrite->setChecked(false);
      this->mpc_Ui->pc_CheckBoxDashboardConnectWrite->setEnabled(false);
   }
   else
   {
      // keep check state as it is, only enable checkbox again
      this->mpc_Ui->pc_CheckBoxDashboardConnectWrite->setEnabled(true);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update spinbox min, max, factor and offset. Necessary e.g. on scaling change.
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeBase::m_UpdateSpinboxMetaData(void) const
{
   const C_OscNodeDataPoolListElement * const pc_Element =
      C_PuiSdHandler::h_GetInstance()->GetOscDataPoolListElement(this->mc_DataElement.u32_NodeIndex,
                                                                 this->mc_DataElement.u32_DataPoolIndex,
                                                                 this->mc_DataElement.u32_ListIndex,
                                                                 this->mc_DataElement.u32_ElementIndex);

   if (pc_Element != NULL)
   {
      const C_OscNodeDataPoolContent c_CurValue = GetDashboardConnectInitialValue();
      const C_PuiSvDbDataElementScaling c_Scaling = this->GetScalingInformation();
      this->mpc_Ui->pc_SpinBoxInitialValue->Init(pc_Element->c_MinValue, pc_Element->c_MaxValue,
                                                 c_Scaling.f64_Factor, c_Scaling.f64_Offset);
      this->m_SetDashboardConnectInitialValue(c_CurValue);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize no data element state
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeBase::m_InitNoDataElement(void) const
{
   this->mpc_Ui->pc_LineEditDataElement->setText("-");
   this->mpc_Ui->pc_LabelUpdateMode->setText("-");

   //Scaling
   this->mpc_Ui->pc_CheckBoxDefaultScaling->setChecked(true);
   this->mpc_Ui->pc_CheckBoxDefaultScaling->setEnabled(false);
   this->mpc_Ui->pc_PushButtonUpdateModeConfigure->setVisible(true);
   m_OnUseDefaultScalingChange();

   //Formatter
   this->mpc_Ui->pc_CheckBoxFormatterActive->setEnabled(false);
   this->mpc_Ui->pc_CheckBoxFormatterActive->setChecked(false);
   this->mpc_Ui->pc_LineEditFormatterString->setText("");
   this->m_OnFormatterActiveChange();

   //Update mode
   if (this->mq_ReadElement == false)
   {
      this->mpc_Ui->pc_ComboBoxTransmissionMode->setCurrentIndex(C_SyvDaPeBase::mhs32_INDEX_MANUAL);
      this->mpc_Ui->pc_ComboBoxTransmissionMode->setEnabled(false);
   }
   else
   {
      this->mpc_Ui->pc_PushButtonUpdateModeConfigure->setEnabled(false);
   }

   //Dashboard connect handling
   this->mpc_Ui->pc_CheckBoxDashboardConnectWrite->setEnabled(false);
   this->mpc_Ui->pc_ComboBoxInitialValueMode->setEnabled(false);
   this->mpc_Ui->pc_ComboBoxInitialValueMode->setCurrentIndex(mhs32_DASHBOARD_CONNECT_DISABLED);
   m_OnInitialValueModeChange(mhs32_DASHBOARD_CONNECT_DISABLED);
   m_UpdateSpinboxMetaData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize valid data element state

   \param[in]  orc_Id               Element id to use for default initialization (only used if not invalid)
   \param[in]  orc_Scaling          Scaling information for default initialization
   \param[in]  orc_FormatterConfig  Formatter information for default initialization
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaPeBase::m_InitDataElement(const C_PuiSvDbNodeDataPoolListElementId & orc_Id,
                                      const C_PuiSvDbDataElementScaling & orc_Scaling,
                                      const C_PuiSvDbDataElementDisplayFormatter & orc_FormatterConfig)
const
{
   if (orc_Id.GetIsValid() == true)
   {
      const QString c_UpdateModeInfo = C_SyvUtil::h_GetUpdateModeDescription(this->mu32_ViewIndex, orc_Id);
      const QString c_Edit = C_PuiSvHandler::h_GetNamespace(orc_Id);

      this->mpc_Ui->pc_LineEditDataElement->setText(c_Edit);
      this->mpc_Ui->pc_LineEditDataElement->SetToolTipInformation(C_GtGetText::h_GetText("Data Element"), c_Edit);
      this->mpc_Ui->pc_LabelNamespaceValue->setText(c_Edit);
      if (this->mc_DisplayName.compare("") == 0)
      {
         this->mpc_Ui->pc_LineEditDataElementDisplayName->setText(this->m_GetDefaultDisplayName(orc_Id));
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

      this->mpc_Ui->pc_CheckBoxFormatterActive->setChecked(orc_FormatterConfig.q_IsActive);
      this->mpc_Ui->pc_LineEditFormatterString->setText(orc_FormatterConfig.c_FormatterString);
      this->m_OnFormatterActiveChange();

      //Update mode
      this->mpc_Ui->pc_LabelUpdateMode->setText(c_UpdateModeInfo);
      this->mpc_Ui->pc_PushButtonUpdateModeConfigure->setVisible(
         orc_Id.GetType() == C_PuiSvDbNodeDataPoolListElementId::eDATAPOOL_ELEMENT);

      // Adapt UI
      this->mpc_Ui->pc_CheckBoxDefaultScaling->setEnabled(true);
      if (this->mq_ReadElement == false)
      {
         this->mpc_Ui->pc_ComboBoxTransmissionMode->setEnabled(true);
      }
      else
      {
         this->mpc_Ui->pc_PushButtonUpdateModeConfigure->setEnabled(true);
      }
      this->mpc_Ui->pc_CheckBoxFormatterActive->setEnabled(true);

      //Dashboard connect handling
      this->mpc_Ui->pc_ComboBoxInitialValueMode->setEnabled(true);
      this->mpc_Ui->pc_CheckBoxDashboardConnectWrite->setEnabled(true);
      m_OnInitialValueModeChange(this->mpc_Ui->pc_ComboBoxInitialValueMode->currentIndex());
      m_UpdateSpinboxMetaData();
   }
   else
   {
      m_InitNoDataElement();
   }
}
