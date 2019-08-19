//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Add undo command (implementation)

   Add undo command

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.h"

#include "gitypes.h"
#include "TGLUtils.h"
#include "stwtypes.h"
#include "constants.h"
#include "stwerrors.h"
#include "C_GtGetText.h"
#include "C_SyvDaDashboardScene.h"
#include "C_SyvDaUnoAddCommand.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw_tgl;
using namespace stw_types;
using namespace stw_errors;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */
//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor for adding a generic element

   \param[in,out] opc_Scene                         Pointer to currently active scene
   \param[in]     orc_IDs                           Affected unique IDs
   \param[in]     ore_Type                          Item type
   \param[in]     orc_NewPos                        New position
   \param[in]     of64_ZValue                       Z value
   \param[in]     orc_AdditionalInformation         Additional string information
   \param[in,out] opc_Parent                        Optional pointer to parent
   \param[in]     orq_ForceUseAdditionalInformation Optional flag to force use of additional string information
   \param[in]     orq_DarkModeDefault               Optional flag to change default values
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaUnoAddCommand::C_SyvDaUnoAddCommand(QGraphicsScene * const opc_Scene,
                                           const std::vector<stw_types::uint64> & orc_IDs,
                                           const C_PuiSvDbDataElement::E_Type & ore_Type, const QPointF & orc_NewPos,
                                           const float64 of64_ZValue, const QString & orc_AdditionalInformation,
                                           QUndoCommand * const opc_Parent,
                                           const bool & orq_ForceUseAdditionalInformation,
                                           const bool & orq_DarkModeDefault) :
   C_SyvDaUnoAddBaseCommand(opc_Scene, orc_IDs, "Add drawing element(s)",
                            opc_Parent),
   me_Type(ore_Type),
   mc_NewPos(orc_NewPos),
   mf64_ZValue(of64_ZValue),
   mc_AdditionalInformation(orc_AdditionalInformation),
   mq_ForceUseAdditionalInformation(orq_ForceUseAdditionalInformation),
   mq_DarkModeDefault(orq_DarkModeDefault)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaUnoAddCommand::~C_SyvDaUnoAddCommand(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add elements without previous knowledge
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaUnoAddCommand::m_AddNew(void)
{
   //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
   C_SyvDaDashboardScene * const pc_Scene = dynamic_cast<C_SyvDaDashboardScene * const>(mpc_Scene);

   if (pc_Scene != NULL)
   {
      const std::vector<stw_types::uint64> c_IDs = this->GetIDs();
      if (c_IDs.size() > 0)
      {
         QMap<C_PuiBsTemporaryDataID, uint64> c_IDMap;
         C_PuiSvDashboard c_InitialData;
         C_PuiSvDbLabel c_LabelData;
         C_PuiSvDbParam c_ParamWidgetData;
         C_PuiSvDbPieChart c_PieChartData;
         C_PuiSvDbSpinBox c_SpinBoxData;
         C_PuiSvDbSlider c_SliderData;
         C_PuiSvDbProgressBar c_ProgressBarData;
         C_PuiSvDbChart c_ChartData;
         C_PuiSvDbTable c_TableData;
         C_PuiSvDbToggle c_ToggleData;
         C_PuiBsLineArrow c_LineArrowData;
         C_PuiBsBoundary c_BoundaryData;
         C_PuiBsTextElement c_TextData;
         C_PuiBsImage c_ImageData;
         switch (this->me_Type)
         {
         case C_PuiSvDbDataElement::eLABEL:
            //Theme
            mh_DecodeTheme(c_LabelData, this->mc_AdditionalInformation);
            //Specific settings
            c_LabelData.f64_ZOrder = this->mf64_ZValue;
            c_LabelData.c_UIPosition = mc_NewPos;
            c_LabelData.f64_Height = 84.0;
            c_LabelData.f64_Width = 150.0;
            tgl_assert(c_InitialData.AddWidget(&c_LabelData, this->me_Type) == C_NO_ERR);
            break;
         case C_PuiSvDbDataElement::ePARAM:
            //Theme
            //Fix
            c_ParamWidgetData.e_DisplayStyle = C_PuiSvDbWidgetBase::eOPENSYDE;
            //Specific settings
            c_ParamWidgetData.f64_ZOrder = this->mf64_ZValue;
            c_ParamWidgetData.c_UIPosition = mc_NewPos;
            c_ParamWidgetData.f64_Height = 246.0;
            c_ParamWidgetData.f64_Width = 895.0;
            tgl_assert(c_InitialData.AddWidget(&c_ParamWidgetData, this->me_Type) == C_NO_ERR);
            break;
         case C_PuiSvDbDataElement::ePIE_CHART:
            //Theme
            mh_DecodeTheme(c_PieChartData, this->mc_AdditionalInformation);
            //Specific settings
            c_PieChartData.f64_ZOrder = this->mf64_ZValue;
            c_PieChartData.c_UIPosition = mc_NewPos;
            c_PieChartData.f64_Height = 150.0;
            c_PieChartData.f64_Width = 150.0;
            tgl_assert(c_InitialData.AddWidget(&c_PieChartData, this->me_Type) == C_NO_ERR);
            break;
         case C_PuiSvDbDataElement::eSPIN_BOX:
            //Theme
            mh_DecodeTheme(c_SpinBoxData, this->mc_AdditionalInformation);
            //Specific settings
            c_SpinBoxData.f64_ZOrder = this->mf64_ZValue;
            c_SpinBoxData.c_UIPosition = mc_NewPos;
            c_SpinBoxData.f64_Height = 60.0;
            c_SpinBoxData.f64_Width = 200.0;
            c_InitialData.AddWidget(&c_SpinBoxData, this->me_Type);
            break;
         case C_PuiSvDbDataElement::eSLIDER:
            //Theme
            mh_DecodeTheme(c_SliderData, this->mc_AdditionalInformation);
            //Specific settings
            c_SliderData.f64_ZOrder = this->mf64_ZValue;
            c_SliderData.c_UIPosition = mc_NewPos;
            c_SliderData.f64_Height = 40.0;
            c_SliderData.f64_Width = 400.0 * (7.0 / 9.0);
            c_InitialData.AddWidget(&c_SliderData, this->me_Type);
            break;
         case C_PuiSvDbDataElement::eTABLE:
            //Theme
            mh_DecodeTheme(c_TableData, this->mc_AdditionalInformation);
            //Specific settings
            c_TableData.f64_ZOrder = this->mf64_ZValue;
            c_TableData.c_UIPosition = mc_NewPos;
            c_TableData.f64_Height = 120.0;
            c_TableData.f64_Width = 511.0;
            c_InitialData.AddWidget(&c_TableData, this->me_Type);
            break;
         case C_PuiSvDbDataElement::ePROGRESS_BAR:
            //Theme
            mh_DecodeTheme(c_ProgressBarData, this->mc_AdditionalInformation);
            //Specific settings
            c_ProgressBarData.f64_ZOrder = this->mf64_ZValue;
            c_ProgressBarData.c_UIPosition = mc_NewPos;
            c_ProgressBarData.f64_Height = 100.0;
            c_ProgressBarData.f64_Width = 100.0;
            c_ProgressBarData.e_Type = C_PuiSvDbProgressBar::eTYPE_2;
            c_ProgressBarData.e_Alignment = C_PuiSvDbProgressBar::eLEFT;
            c_InitialData.AddWidget(&c_ProgressBarData, this->me_Type);
            break;
         case C_PuiSvDbDataElement::eCHART:
            //Theme
            mh_DecodeTheme(c_ChartData, this->mc_AdditionalInformation);
            //Specific settings
            c_ChartData.f64_ZOrder = this->mf64_ZValue;
            c_ChartData.c_UIPosition = mc_NewPos;
            c_ChartData.f64_Height = 300.0;
            c_ChartData.f64_Width = 600.0;
            c_InitialData.AddWidget(&c_ChartData, this->me_Type);
            break;
         case C_PuiSvDbDataElement::eTOGGLE:
            //Theme
            mh_DecodeTheme(c_ToggleData, this->mc_AdditionalInformation);
            //Specific settings
            c_ToggleData.f64_ZOrder = this->mf64_ZValue;
            c_ToggleData.c_UIPosition = mc_NewPos;
            c_ToggleData.f64_Height = 50.0;
            c_ToggleData.f64_Width = 83.0;
            c_InitialData.AddWidget(&c_ToggleData, this->me_Type);
            break;
         case C_PuiSvDbDataElement::eLINE_ARROW:
            c_LineArrowData.f64_ZOrder = this->mf64_ZValue;
            c_LineArrowData.c_UIInteractionPoints.push_back(mc_NewPos);
            c_LineArrowData.c_UIInteractionPoints.push_back(mc_NewPos + QPointF(200.0, 0.0));
            c_InitialData.AddLineArrow(c_LineArrowData);
            break;
         case C_PuiSvDbDataElement::eBOUNDARY:
            c_BoundaryData.c_UIPosition = mc_NewPos;
            c_BoundaryData.f64_Height = 100.0;
            c_BoundaryData.f64_Width = 100.0;
            c_BoundaryData.f64_ZOrder = this->mf64_ZValue;
            c_InitialData.AddBoundary(c_BoundaryData);
            break;
         case C_PuiSvDbDataElement::eTEXT_ELEMENT:
            c_TextData.f64_ZOrder = this->mf64_ZValue;
            c_TextData.c_UIPosition = mc_NewPos;
            c_TextData.f64_Height = 50.0;
            c_TextData.f64_Width = 50.0;
            c_TextData.c_UIText = this->mc_AdditionalInformation;
            c_InitialData.AddTextElement(c_TextData);
            break;
         case C_PuiSvDbDataElement::eIMAGE:
            if (this->mc_AdditionalInformation.compare("") != 0)
            {
               QPixmap c_Image(this->mc_AdditionalInformation);
               c_ImageData.c_UIPosition = mc_NewPos;
               c_ImageData.f64_Height = c_Image.height();
               c_ImageData.f64_Width = c_Image.width();
               c_ImageData.c_UIImagePixmap = c_Image;
               c_ImageData.f64_ZOrder = this->mf64_ZValue;
               c_InitialData.AddImage(c_ImageData);
            }
            break;
         case C_PuiSvDbDataElement::eUNKNOWN:
         default:
            //No element to create
            break;
         }
         //ID map
         c_IDMap.insert(C_PuiBsTemporaryDataID(static_cast<sint32>(this->me_Type), 0), c_IDs[0]);
         pc_Scene->CopyFromSnapshotToScene(c_InitialData, &c_IDMap);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Decode theme string and assign

   \param[in,out] orc_Base  Base widget data to assign theme to
   \param[in]     orc_Theme Theme string
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaUnoAddCommand::mh_DecodeTheme(C_PuiSvDbWidgetBase & orc_Base, const QString & orc_Theme)
{
   const QString c_Theme1 = C_GtGetText::h_GetText("openSYDE");
   const QString c_Theme2 = C_GtGetText::h_GetText("Material");
   const QString c_Theme3 = C_GtGetText::h_GetText("Flat");
   const QString c_Theme4 = C_GtGetText::h_GetText("Skeuomorphic");

   if (orc_Theme.compare(c_Theme1) == 0)
   {
      orc_Base.e_DisplayStyle = C_PuiSvDbWidgetBase::eOPENSYDE;
   }
   else if (orc_Theme.compare(c_Theme2) == 0)
   {
      orc_Base.e_DisplayStyle = C_PuiSvDbWidgetBase::eOPENSYDE_2;
   }
   else if (orc_Theme.compare(c_Theme3) == 0)
   {
      orc_Base.e_DisplayStyle = C_PuiSvDbWidgetBase::eFLAT;
   }
   else if (orc_Theme.compare(c_Theme4) == 0)
   {
      orc_Base.e_DisplayStyle = C_PuiSvDbWidgetBase::eSKEUOMORPH;
   }
   else
   {
      tgl_assert(false);
   }
}
