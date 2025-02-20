//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Main graphics scene for a concrete dashboard (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QGraphicsView>
#include <QMimeData>

#include "C_GiWiProxyBase.hpp"

#include "gitypes.hpp"
#include "stwtypes.hpp"
#include "stwerrors.hpp"

#include "TglUtils.hpp"
#include "C_GiSvDaArrow.hpp"
#include "C_GiSvDaBoundary.hpp"
#include "C_GiSvDaLabelBase.hpp"
#include "C_GiSvDaParam.hpp"
#include "C_GiSvDaPieChartBase.hpp"
#include "C_GiSvDaSpinBoxBase.hpp"
#include "C_GiSvDaSliderBase.hpp"
#include "C_GiSvDaProgressBarBase.hpp"
#include "C_GiSvDaTableBase.hpp"
#include "C_GiSvDaToggleBase.hpp"
#include "C_GiSvDaImageGroup.hpp"
#include "C_GiSvDaTextElement.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_SyvDaDashboardScene.hpp"
#include "C_SdTopologyListWidget.hpp"
#include "C_GtGetText.hpp"
#include "C_SyvClipBoardHelper.hpp"
#include "C_OgeWiCustomMessage.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::tgl;
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_gui_elements;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in]      ou32_ViewIndex         View index
   \param[in]      ou32_DashboardIndex    Dashboard index
   \param[in]      oq_LoadDashboard       Optional flag to avoid loading any items initially
   \param[in,out]  opc_Parent             Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SyvDaDashboardScene::C_SyvDaDashboardScene(const uint32_t ou32_ViewIndex, const uint32_t ou32_DashboardIndex,
                                             const bool oq_LoadDashboard, QObject * const opc_Parent) :
   C_SebScene(opc_Parent),
   mc_UndoManager(this),
   mq_EditMode(false),
   mu32_ViewIndex(ou32_ViewIndex),
   mu32_DashboardIndex(ou32_DashboardIndex),
   mq_LastKnownDarkMode(false),
   mq_DarkModeInitialized(false)
{
   // Init base scene. Initializing all parts which can not be initilized in the base constructor
   this->m_InitSceneContextMenuManager();

   this->mc_CopyPasteManager.PrepareCopyFromSceneToManager(this->mu32_ViewIndex, this->mu32_DashboardIndex);

   // testing
   if (oq_LoadDashboard == true)
   {
      this->Load();
   }

   //Init context menu
   this->m_InitBaseSceneContextMenuManager();

   // selection changed
   connect(this, &C_SyvDaDashboardScene::selectionChanged, this, &C_SyvDaDashboardScene::m_SelectionChanged);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_TestPointer and the Qt memory management
C_SyvDaDashboardScene::~C_SyvDaDashboardScene(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checks if the cursor position is relevant for the proxy widget

   Checks all widgets of base type C_GiSvDaRectBaseGroup.

   \param[in]  orc_ScenePos   Scene position of cursor

   \return
   true     Cursor is on a relevant position
   false    Cursor is not on a relevant position
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaDashboardScene::IsMousePosRelevantForProxyWidgetInteraction(const QPointF & orc_ScenePos)
{
   bool q_Return = false;
   const QList<QGraphicsItem *> & rc_Items = this->items();

   // update the items
   for (QList<QGraphicsItem *>::const_iterator c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
   {
      C_GiSvDaRectBaseGroup * const pc_Item = dynamic_cast<C_GiSvDaRectBaseGroup *>(*c_ItItem);
      if (pc_Item != NULL)
      {
         if ((this->mq_EditMode == false) ||
             (pc_Item->IsEditContentEnabled() == true))
         {
            q_Return = pc_Item->IsMousePosRelevantForProxyWidgetInteraction(orc_ScenePos);
            if (q_Return == true)
            {
               break;
            }
         }
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Checks if the current selection is relevant for the proxy widget

   In base is no item relevant. Function returns always false.
   Derived classes must overwrite the function if needed.

   \return
   true     Selection is on a relevant position
   false    Selection is not on a relevant position
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaDashboardScene::IsSelectionRelevantForProxyWidgetInteraction(void) const
{
   bool q_Return = false;
   const QList<QGraphicsItem *> & rc_Items = this->items();

   // update the items
   for (QList<QGraphicsItem *>::const_iterator c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
   {
      C_GiSvDaRectBaseGroup * const pc_Item = dynamic_cast<C_GiSvDaRectBaseGroup *>(*c_ItItem);
      if (pc_Item != NULL)
      {
         q_Return = pc_Item->IsEditContentEnabled();
         if (q_Return == true)
         {
            break;
         }
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the necessary cursor type when hovered dependent of the cursor position

   \param[in]       orc_ScenePos     Scene position of cursor
   \param[out]      ore_Cursor       Detected necessary cursor

   \retval   true   Override cursor is necessary
   \retval   false  Override cursor is not necessary
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaDashboardScene::IsOverrideCursorNecessary(const QPointF & orc_ScenePos, Qt::CursorShape & ore_Cursor) const
{
   bool q_Return = false;

   if (this->mq_EditMode == true)
   {
      QList<QGraphicsItem *>::const_iterator c_ItItem;
      const QList<QGraphicsItem *> & rc_Items = this->items();
      float64_t f64_HighestZeOrderUnderCursor = 0.0;
      bool q_AtLeastOneFound = false;

      // Reset cursor to default
      ore_Cursor = Qt::SizeAllCursor;

      // search a item which lies under the cursor
      for (c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
      {
         C_GiSvDaRectBaseGroup * const pc_Item = dynamic_cast<C_GiSvDaRectBaseGroup *>(*c_ItItem);

         if ((pc_Item != NULL) &&
             (pc_Item->isUnderMouse() == true))
         {
            Qt::CursorShape e_TempCursor;
            q_Return = pc_Item->IsOverrideCursorNecessary(orc_ScenePos, e_TempCursor);

            if (q_Return == true)
            {
               // Get the necessary cursor of the highest element with the z order to have the cursor which
               // does match to real caused function when clicked
               if ((q_AtLeastOneFound == false) ||
                   (pc_Item->zValue() > f64_HighestZeOrderUnderCursor))
               {
                  // First item found or an item above the previous item in the z order
                  f64_HighestZeOrderUnderCursor = pc_Item->zValue();
                  ore_Cursor = e_TempCursor;
                  q_AtLeastOneFound = true;
               }
            }
         }
      }
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the state of the edit mode

   \param[in]  oq_Active   Flag for setting the edit mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::SetEditMode(const bool oq_Active)
{
   const QList<QGraphicsItem *> & rc_Items = this->items();

   this->mq_EditMode = oq_Active;

   //Handle elements for switching modes
   for (QList<QGraphicsItem *>::const_iterator c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
   {
      this->m_SetItemSelectionAndMoveAvailability(*c_ItItem);
   }

   // update the items
   for (QList<QGraphicsItem *>::const_iterator c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
   {
      C_GiSvDaRectBaseGroup * const pc_Item = dynamic_cast<C_GiSvDaRectBaseGroup *>(*c_ItItem);

      C_GiBiTextElement * const pc_TextItem = dynamic_cast<C_GiBiTextElement *>(*c_ItItem);

      if (pc_Item != NULL)
      {
         pc_Item->EditModeActiveChanged(oq_Active);
      }

      if (pc_TextItem != NULL)
      {
         pc_TextItem->SetEditable(oq_Active);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Function to activate or deactivate drawing of performance heavy widgets

   \param[in]  oq_Active   Flag if widgets should currently be drawn
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::SetDrawingActive(const bool oq_Active) const
{
   const QList<QGraphicsItem *> & rc_Items = this->items();

   // inform the items
   for (QList<QGraphicsItem *>::const_iterator c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
   {
      QGraphicsItem * const pc_Parent = C_SebUtil::h_GetHighestParent(*c_ItItem);

      C_GiSvDaRectBaseGroup * const pc_DataElement = dynamic_cast<C_GiSvDaRectBaseGroup *>(pc_Parent);
      if (pc_DataElement != NULL)
      {
         pc_DataElement->SetDrawingActive(oq_Active);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dashboard index

   \param[in]  ou32_DashboardIndex  New dashboard index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::SetDashboardIndex(const uint32_t ou32_DashboardIndex)
{
   const QList<QGraphicsItem *> & rc_Items = this->items();

   this->mu32_DashboardIndex = ou32_DashboardIndex;

   this->mc_CopyPasteManager.PrepareCopyFromSceneToManager(this->mu32_ViewIndex, this->mu32_DashboardIndex);

   // inform the items
   for (QList<QGraphicsItem *>::const_iterator c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
   {
      QGraphicsItem * const pc_Parent = C_SebUtil::h_GetHighestParent(*c_ItItem);

      C_PuiSvDbDataElement * const pc_DataElement = dynamic_cast<C_PuiSvDbDataElement *>(pc_Parent);
      if (pc_DataElement != NULL)
      {
         pc_DataElement->SetDashboardIndex(ou32_DashboardIndex);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load dashboard widgets
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::Load(void)
{
   //Fill up with zeros
   const QVector<uint32_t> c_SaveIndices(13, 0);

   //Sync system definition scaling information
   tgl_assert(C_PuiSvHandler::h_GetInstance()->SyncDashboardScalingInformation(this->mu32_ViewIndex,
                                                                               this->mu32_DashboardIndex) == C_NO_ERR);

   this->m_LoadSubset(c_SaveIndices, false, NULL);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::Save(void) const
{
   const QList<QGraphicsItem *> & rc_Items = this->items();

   // inform the items
   for (QList<QGraphicsItem *>::const_iterator c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
   {
      // TODO Watch if parent necessary

      C_PuiSvDbDataElement * const pc_DataElement = dynamic_cast<C_PuiSvDbDataElement *>(*c_ItItem);
      if (pc_DataElement != NULL)
      {
         pc_DataElement->UpdateData();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy snapshot to scene

   \param[in]  orc_Snapshot   Object snapshot
   \param[in]  opc_IdMap      Optional map for IDs to use
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::CopyFromSnapshotToScene(const C_PuiSvDashboard & orc_Snapshot,
                                                    const QMap<C_PuiBsTemporaryDataId,
                                                               uint64_t> * const opc_IdMap)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(this->mu32_DashboardIndex);
      if (pc_Dashboard != NULL)
      {
         uint32_t u32_ItElem;
         const std::vector<C_PuiSvDbLabel> & rc_Labels = orc_Snapshot.GetLabels();
         const std::vector<C_PuiSvDbParam> & rc_Params = orc_Snapshot.GetParams();
         const std::vector<C_PuiSvDbPieChart> & rc_PieCharts = orc_Snapshot.GetPieCharts();
         const std::vector<C_PuiSvDbProgressBar> & rc_ProgressBars = orc_Snapshot.GetProgressBars();
         const std::vector<C_PuiSvDbSlider> & rc_Sliders = orc_Snapshot.GetSliders();
         const std::vector<C_PuiSvDbSpinBox> & rc_SpinBoxes = orc_Snapshot.GetSpinBoxes();
         const std::vector<C_PuiSvDbTable> & rc_Tables = orc_Snapshot.GetTables();
         const std::vector<C_PuiSvDbToggle> & rc_Toggles = orc_Snapshot.GetToggles();
         /*
         0: Boundary
         1: Text element
         2: Line arrow
         3: Image
         4: Label
         5: Spin box
         6: Slider
         7: Progress bar
         8: Toggle
         9: Pie chart
         10: Table
         11: Param
         */
         QVector<uint32_t> c_OtherIndices;
         //Indices
         c_OtherIndices.push_back(pc_Dashboard->c_Boundaries.size());
         c_OtherIndices.push_back(pc_Dashboard->c_TextElements.size());
         c_OtherIndices.push_back(pc_Dashboard->c_LineArrows.size());
         c_OtherIndices.push_back(pc_Dashboard->c_Images.size());
         c_OtherIndices.push_back(pc_Dashboard->GetLabels().size());
         c_OtherIndices.push_back(pc_Dashboard->GetSpinBoxes().size());
         c_OtherIndices.push_back(pc_Dashboard->GetSliders().size());
         c_OtherIndices.push_back(pc_Dashboard->GetProgressBars().size());
         c_OtherIndices.push_back(pc_Dashboard->GetToggles().size());
         c_OtherIndices.push_back(pc_Dashboard->GetPieCharts().size());
         c_OtherIndices.push_back(pc_Dashboard->GetTables().size());
         c_OtherIndices.push_back(pc_Dashboard->GetParams().size());
         //Ignore deprecated charts
         //Copy labels
         for (u32_ItElem = 0; u32_ItElem < rc_Labels.size(); ++u32_ItElem)
         {
            tgl_assert(C_PuiSvHandler::h_GetInstance()->AddDashboardWidget(this->mu32_ViewIndex,
                                                                           this->mu32_DashboardIndex,
                                                                           &rc_Labels[u32_ItElem],
                                                                           C_PuiSvDbDataElement::eLABEL) == C_NO_ERR);
         }
         //Copy param widgets
         for (u32_ItElem = 0; u32_ItElem < rc_Params.size(); ++u32_ItElem)
         {
            tgl_assert(C_PuiSvHandler::h_GetInstance()->AddDashboardWidget(this->mu32_ViewIndex,
                                                                           this->mu32_DashboardIndex,
                                                                           &rc_Params[u32_ItElem],
                                                                           C_PuiSvDbDataElement::ePARAM) == C_NO_ERR);
         }
         //Copy pie charts
         for (u32_ItElem = 0; u32_ItElem < rc_PieCharts.size(); ++u32_ItElem)
         {
            tgl_assert(C_PuiSvHandler::h_GetInstance()->AddDashboardWidget(this->mu32_ViewIndex,
                                                                           this->mu32_DashboardIndex,
                                                                           &rc_PieCharts[u32_ItElem],
                                                                           C_PuiSvDbDataElement::ePIE_CHART) ==
                       C_NO_ERR);
         }
         //Copy progress bars
         for (u32_ItElem = 0; u32_ItElem < rc_ProgressBars.size(); ++u32_ItElem)
         {
            tgl_assert(C_PuiSvHandler::h_GetInstance()->AddDashboardWidget(this->mu32_ViewIndex,
                                                                           this->mu32_DashboardIndex,
                                                                           &rc_ProgressBars[u32_ItElem],
                                                                           C_PuiSvDbDataElement::ePROGRESS_BAR) ==
                       C_NO_ERR);
         }
         //Copy sliders
         for (u32_ItElem = 0; u32_ItElem < rc_Sliders.size(); ++u32_ItElem)
         {
            tgl_assert(C_PuiSvHandler::h_GetInstance()->AddDashboardWidget(this->mu32_ViewIndex,
                                                                           this->mu32_DashboardIndex,
                                                                           &rc_Sliders[u32_ItElem],
                                                                           C_PuiSvDbDataElement::eSLIDER) == C_NO_ERR);
         }
         //Copy spin boxes
         for (u32_ItElem = 0; u32_ItElem < rc_SpinBoxes.size(); ++u32_ItElem)
         {
            tgl_assert(C_PuiSvHandler::h_GetInstance()->AddDashboardWidget(this->mu32_ViewIndex,
                                                                           this->mu32_DashboardIndex,
                                                                           &rc_SpinBoxes[u32_ItElem],
                                                                           C_PuiSvDbDataElement::eSPIN_BOX) ==
                       C_NO_ERR);
         }
         //Copy tables
         for (u32_ItElem = 0; u32_ItElem < rc_Tables.size(); ++u32_ItElem)
         {
            tgl_assert(C_PuiSvHandler::h_GetInstance()->AddDashboardWidget(this->mu32_ViewIndex,
                                                                           this->mu32_DashboardIndex,
                                                                           &rc_Tables[u32_ItElem],
                                                                           C_PuiSvDbDataElement::eTABLE) == C_NO_ERR);
         }
         //Copy toggles
         for (u32_ItElem = 0; u32_ItElem < rc_Toggles.size(); ++u32_ItElem)
         {
            tgl_assert(C_PuiSvHandler::h_GetInstance()->AddDashboardWidget(this->mu32_ViewIndex,
                                                                           this->mu32_DashboardIndex,
                                                                           &rc_Toggles[u32_ItElem],
                                                                           C_PuiSvDbDataElement::eTOGGLE) ==
                       C_NO_ERR);
         }
         //Copy other elements
         for (u32_ItElem = 0; u32_ItElem < orc_Snapshot.c_Boundaries.size(); ++u32_ItElem)
         {
            C_PuiSvHandler::h_GetInstance()->AddDashboardBoundary(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                                  orc_Snapshot.c_Boundaries[u32_ItElem]);
         }
         for (u32_ItElem = 0; u32_ItElem < orc_Snapshot.c_TextElements.size(); ++u32_ItElem)
         {
            C_PuiSvHandler::h_GetInstance()->AddDashboardTextElement(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                                     orc_Snapshot.c_TextElements[
                                                                        u32_ItElem]);
         }
         for (u32_ItElem = 0; u32_ItElem < orc_Snapshot.c_LineArrows.size(); ++u32_ItElem)
         {
            C_PuiSvHandler::h_GetInstance()->AddDashboardLineArrow(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                                   orc_Snapshot.c_LineArrows[
                                                                      u32_ItElem]);
         }
         for (u32_ItElem = 0; u32_ItElem < orc_Snapshot.c_Images.size(); ++u32_ItElem)
         {
            C_PuiSvHandler::h_GetInstance()->AddDashboardImage(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                                               orc_Snapshot.c_Images[u32_ItElem]);
         }
         m_LoadSubset(c_OtherIndices, true, opc_IdMap);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Clear item

   Hint: leaves all items which are not included in the system view dashboard

   \param[in,out]  opc_Item   Pointer to item which may be deleted
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::DeleteItem(QGraphicsItem * const opc_Item)
{
   // if the selected item is a bus, the bus class itself is the parent
   QGraphicsItem * const pc_Item = C_SebUtil::h_GetHighestParent(opc_Item);

   if (pc_Item != NULL)
   {
      C_PuiBsDataElement * const pc_DataElement = dynamic_cast<C_PuiBsDataElement *>(pc_Item);

      QObject * const pc_Object = dynamic_cast<QObject *>(pc_Item);

      const C_GiSvDaRectBaseGroup * const pc_Widget = dynamic_cast<C_GiSvDaRectBaseGroup *>(pc_Item);
      if (pc_Widget != NULL)
      {
         m_RemoveWidgetOfScene(pc_Widget);
      }
      else
      {
         const C_GiSvDaBoundary * const pc_Boundary = dynamic_cast<C_GiSvDaBoundary *>(pc_Item);
         if (pc_Boundary != NULL)
         {
            m_RemoveBoundaryOfScene(pc_Boundary);
         }
         else
         {
            const C_GiSvDaTextElement * const pc_TextElement = dynamic_cast<C_GiSvDaTextElement *>(pc_Item);
            if (pc_TextElement != NULL)
            {
               m_RemoveTextElementOfScene(pc_TextElement);
            }
            else
            {
               const C_GiSvDaImageGroup * const pc_ImageGroup = dynamic_cast<C_GiSvDaImageGroup *>(pc_Item);
               if (pc_ImageGroup != NULL)
               {
                  m_RemoveImageGroupOfScene(pc_ImageGroup);
               }
               else
               {
                  const C_GiSvDaArrow * const pc_Arrow = dynamic_cast<C_GiSvDaArrow *>(pc_Item);
                  if (pc_Arrow != NULL)
                  {
                     m_RemoveLineArrowOfScene(pc_Arrow);
                  }
               }
            }
         }
      }

      // common data delete function
      if (pc_DataElement != NULL)
      {
         pc_DataElement->DeleteData();
      }

      // remove from scene TODO: investigate delete bug
      //this->removeItem(pc_Item);

      // common object clean up
      if (pc_Object != NULL)
      {
         pc_Object->deleteLater();
      }
      else
      {
         //Delete
         delete (pc_Item);
      }

      //Clean up tool tip if any existing
      this->m_HandleHideToolTip();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set dark mode active

   \param[in]  oq_Value    Dark mode active
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::SetDarkModeActive(const bool oq_Value)
{
   const QList<QGraphicsItem *> & rc_Items = this->items();

   if ((this->mq_DarkModeInitialized == true) && (this->mq_LastKnownDarkMode != oq_Value))
   {
      //Change dark mode state to save in last known state
      C_PuiSvHandler::h_GetInstance()->SetViewDarkModeActive(this->mu32_ViewIndex, this->mq_LastKnownDarkMode);
      this->Save();
      C_PuiSvHandler::h_GetInstance()->SetViewDarkModeActive(this->mu32_ViewIndex, oq_Value);
      this->mc_UndoManager.clear();
      this->mq_LastKnownDarkMode = oq_Value;
   }
   C_SebScene::SetDarkModeActive(oq_Value);

   // inform the items
   for (QList<QGraphicsItem *>::const_iterator c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
   {
      // TODO Watch if parent necessary

      C_GiSvDaRectBaseGroup * const pc_RectBase = dynamic_cast<C_GiSvDaRectBaseGroup *>(*c_ItItem);
      if (pc_RectBase != NULL)
      {
         pc_RectBase->SetDisplayStyle(pc_RectBase->GetDisplayStyleType(), oq_Value);
      }
      else
      {
         C_PuiSvDbDataElement * const pc_OtherElement = dynamic_cast<C_PuiSvDbDataElement * const>(*c_ItItem);
         //Reload data & dark mode for basic drawing items
         if (pc_OtherElement != NULL)
         {
            pc_OtherElement->LoadData();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Activate dark mode switch functionality
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::SetDarkModeInitialized(void)
{
   if (this->mq_DarkModeInitialized == false)
   {
      const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

      if (pc_View != NULL)
      {
         this->mq_LastKnownDarkMode = pc_View->GetDarkModeActive();
      }
      this->mq_DarkModeInitialized = true;
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Update text

   Necessary because of issue #49525
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::UpdateBoundaries(void) const
{
   const QList<QGraphicsItem *> & rc_Items = this->items();

   for (QList<QGraphicsItem *>::const_iterator c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end();
        ++c_ItItem)
   {
      C_GiSvDaBoundary * const pc_Item = dynamic_cast< C_GiSvDaBoundary * const>(*c_ItItem);
      if (pc_Item != NULL)
      {
         pc_Item->update();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy items to scene from copy paste manager

   \param[in]  opc_Pos  Optional position offset
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::CopyFromManagerToScene(const QPointF * const opc_Pos)
{
   QGraphicsView * const pc_View = this->views().at(0);
   const C_PuiBsElements * const pc_Data = this->mc_CopyPasteManager.GetSnapshot(pc_View);

   const C_SyvDaDashboardSnapshot * const pc_SnapShot =
      dynamic_cast<const C_SyvDaDashboardSnapshot * const>(pc_Data);
   const QMap<stw::opensyde_core::C_OscNodeDataPoolListElementId,
              C_PuiSvReadDataConfiguration> * const pc_Rails = this->mc_CopyPasteManager.GetRails();

   if ((pc_SnapShot != NULL) && (pc_Rails != NULL))
   {
      const QPointF c_OFFSET = QPointF(10.0, 10.0);
      const uint32_t u32_ItemCount = pc_SnapShot->Count();

      std::vector<uint64_t> c_UniqueIds;
      QPointF c_TotalOffset;
      if (opc_Pos == NULL)
      {
         //Add point offset
         this->mc_CopyPasteManager.IncrementPasteCounter();
         c_TotalOffset = static_cast<float64_t>(this->mc_CopyPasteManager.GetPasteCounter()) * c_OFFSET;
      }
      else
      {
         //Set absolute position
         c_TotalOffset = this->mc_CopyPasteManager.GetDiff(*opc_Pos);
      }
      c_UniqueIds.resize(u32_ItemCount);
      for (uint32_t u32_ItItem = 0; u32_ItItem < u32_ItemCount; ++u32_ItItem)
      {
         c_UniqueIds[u32_ItItem] = m_GetNewUniqueId();
      }
      this->mc_UndoManager.DoAddSnapshot(c_UniqueIds, *pc_SnapShot, *pc_Rails, c_TotalOffset, this->GetHighestUsedZetValueList(
                                            this->items()));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Signal for update of current scaling

   \param[in]  orc_Transform  Current scaling
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::UpdateTransform(const QTransform & orc_Transform)
{
   C_SebScene::UpdateTransform(orc_Transform);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking if any item can be added to the scene

   \return
   true     Items can be added
   false    Items cannot be added
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaDashboardScene::IsAnyItemAddable(void) const
{
   return this->mq_EditMode;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking if the graphics item is movable on the scene

   \param[in]  opc_Item    Item to check

   \return
   true     Item is movable
   false    Item is not movable
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaDashboardScene::IsItemMovable(const QGraphicsItem * const opc_Item) const
{
   bool q_Return;

   // no restrictions for moving elements
   // list all items (invalid items should have unknown type)
   const int32_t s32_Type = opc_Item->type();

   if ((s32_Type == ms32_GRAPHICS_ITEM_DB_LABEL) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_SPIN_BOX) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_PROGRESS_BAR) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_SLIDER) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_TOGGLE) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_PIE_CHART) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_TABLE) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_PARAM) ||
       (s32_Type == ms32_GRAPHICS_ITEM_TEXTELEMENT) ||
       (s32_Type == ms32_GRAPHICS_ITEM_LINE_ARROW) ||
       (s32_Type == ms32_GRAPHICS_ITEM_BOUNDARY) ||
       (s32_Type == ms32_GRAPHICS_ITEM_IMAGE))
   {
      //If correct item type use the edit mode flag
      q_Return = this->mq_EditMode;
   }
   else
   {
      //Default
      q_Return = false;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking if the graphics item is selectable on the scene

   \param[in]  opc_Item    Item to check

   \return
   true     Item is selectable
   false    Item is not selectable
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaDashboardScene::IsItemSelectable(const QGraphicsItem * const opc_Item) const
{
   bool q_Return;

   // no restrictions for selecting elements
   // list all items (invalid items should have unknown type)
   const int32_t s32_Type = opc_Item->type();

   if ((s32_Type == ms32_GRAPHICS_ITEM_DB_LABEL) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_SPIN_BOX) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_PROGRESS_BAR) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_SLIDER) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_TOGGLE) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_PIE_CHART) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_TABLE) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_PARAM) ||
       (s32_Type == ms32_GRAPHICS_ITEM_TEXTELEMENT) ||
       (s32_Type == ms32_GRAPHICS_ITEM_LINE_ARROW) ||
       (s32_Type == ms32_GRAPHICS_ITEM_BOUNDARY) ||
       (s32_Type == ms32_GRAPHICS_ITEM_IMAGE))
   {
      //If correct item type use the edit mode flag
      q_Return = this->mq_EditMode;
   }
   else
   {
      //Default
      q_Return = false;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking if the graphics item is deletable on the scene

   \param[in]  opc_Item    Item to check

   \return
   true     Item is deletable
   false    Item is not deletable
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaDashboardScene::IsItemDeletable(const QGraphicsItem * const opc_Item) const
{
   bool q_Return;

   // no restrictions for deleting elements
   // list all items (invalid items should have unknown type)
   const int32_t s32_Type = opc_Item->type();

   if ((s32_Type == ms32_GRAPHICS_ITEM_DB_LABEL) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_SPIN_BOX) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_PROGRESS_BAR) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_SLIDER) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_TOGGLE) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_PIE_CHART) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_TABLE) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_PARAM) ||
       (s32_Type == ms32_GRAPHICS_ITEM_TEXTELEMENT) ||
       (s32_Type == ms32_GRAPHICS_ITEM_LINE_ARROW) ||
       (s32_Type == ms32_GRAPHICS_ITEM_BOUNDARY) ||
       (s32_Type == ms32_GRAPHICS_ITEM_IMAGE))
   {
      //If correct item type use the edit mode flag
      q_Return = this->mq_EditMode;
   }
   else
   {
      //Default
      q_Return = false;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking if the graphics item is changeable in the zorder

   \param[in]  opc_Item    Item to check

   \return
   true     Z order is changeable
   false    Z order is not changeable
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaDashboardScene::IsZetOrderChangeable(const QGraphicsItem * const opc_Item) const
{
   bool q_Return;

   // no restrictions for using z order
   // list all items (invalid items should have unknown type)
   const int32_t s32_Type = opc_Item->type();

   if ((s32_Type == ms32_GRAPHICS_ITEM_DB_LABEL) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_SPIN_BOX) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_PROGRESS_BAR) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_SLIDER) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_TOGGLE) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_PIE_CHART) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_TABLE) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_PARAM) ||
       (s32_Type == ms32_GRAPHICS_ITEM_TEXTELEMENT) ||
       (s32_Type == ms32_GRAPHICS_ITEM_LINE_ARROW) ||
       (s32_Type == ms32_GRAPHICS_ITEM_BOUNDARY) ||
       (s32_Type == ms32_GRAPHICS_ITEM_IMAGE))
   {
      //If correct item type use the edit mode flag
      q_Return = this->mq_EditMode;
   }
   else
   {
      //Default
      q_Return = false;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking if the graphics item can be aligned

   \param[in]  opc_Item    Item to check

   \return
   true     Item can be aligned
   false    Item can not be alined
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaDashboardScene::IsAlignmentUsable(const QGraphicsItem * const opc_Item) const
{
   bool q_Return;

   // no restrictions for using alignment
   // list all items (invalid items should have unknown type)
   const int32_t s32_Type = opc_Item->type();

   if ((s32_Type == ms32_GRAPHICS_ITEM_DB_LABEL) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_SPIN_BOX) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_PROGRESS_BAR) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_SLIDER) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_TOGGLE) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_PIE_CHART) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_TABLE) ||
       (s32_Type == ms32_GRAPHICS_ITEM_DB_PARAM) ||
       (s32_Type == ms32_GRAPHICS_ITEM_TEXTELEMENT) ||
       (s32_Type == ms32_GRAPHICS_ITEM_LINE_ARROW) ||
       (s32_Type == ms32_GRAPHICS_ITEM_BOUNDARY) ||
       (s32_Type == ms32_GRAPHICS_ITEM_IMAGE))
   {
      //If correct item type use the edit mode flag
      q_Return = this->mq_EditMode;
   }
   else
   {
      //Default
      q_Return = false;
   }

   return q_Return;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Checking if the drag move over the scene can be used

   \return
   true     Drag move is available
   false    Drag move is available
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaDashboardScene::IsSceneRubberBandAvailable(void) const
{
   return this->mq_EditMode;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get dashboard index

   \return
   Current dashboard index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SyvDaDashboardScene::GetDashboardIndex(void) const
{
   return this->mu32_DashboardIndex;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Registers all relevant dashboard widgets at the associated data dealer

   \param[in,out]  orc_ComDriver    Com driver containing information about all data dealer
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::RegisterWidgets(C_SyvComDriverDiag & orc_ComDriver) const
{
   const QList<QGraphicsItem *> & rc_Items = this->items();
   const std::vector<C_SyvComDataDealer *> & rc_AllDataDealer = orc_ComDriver.GetAllDataDealer();

   // update the items
   for (QList<QGraphicsItem *>::const_iterator c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
   {
      C_PuiSvDbDataElementHandler * const pc_Item = dynamic_cast<C_PuiSvDbDataElementHandler *>(*c_ItItem);

      C_GiSvDaParam * const pc_Param = dynamic_cast<C_GiSvDaParam *>(*c_ItItem);

      if (pc_Item != NULL)
      {
         uint32_t u32_Counter;

         // Registration of datapool elements
         for (u32_Counter = 0U; u32_Counter < rc_AllDataDealer.size(); ++u32_Counter)
         {
            rc_AllDataDealer[u32_Counter]->RegisterWidget(pc_Item);
         }

         // Registration of datapool com signals
         orc_ComDriver.RegisterWidget(pc_Item);
      }

      // Special case: Parameterization widget needs the entire SyvComDriver
      if (pc_Param != NULL)
      {
         pc_Param->SetSyvComDriver(orc_ComDriver);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Information about the start or stop of a connection

   \param[in]  oq_Active   Flag if connection is active or not active now
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::ConnectionActiveChanged(const bool oq_Active) const
{
   const QList<QGraphicsItem *> & rc_Items = this->items();

   // inform the items
   for (QList<QGraphicsItem *>::const_iterator c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
   {
      C_GiSvDaRectBaseGroup * const pc_Item = dynamic_cast<C_GiSvDaRectBaseGroup *>(*c_ItItem);
      if (pc_Item != NULL)
      {
         pc_Item->ConnectionActiveChanged(oq_Active);
      }
   }

   if (oq_Active == true)
   {
      // In case of an activated connection a second call is necessary
      // All widgets must handled ConnectionActiveChanged before calling this function to avoid race conditions
      // between initial read and write configurations of the write widgets
      for (QList<QGraphicsItem *>::const_iterator c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
      {
         C_GiSvDaRectBaseGroup * const pc_Item = dynamic_cast<C_GiSvDaRectBaseGroup *>(*c_ItItem);
         if (pc_Item != NULL)
         {
            pc_Item->ConnectionActiveStarted();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Updates all values of all dashboard widgets
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::UpdateShowValues(void) const
{
   const QList<QGraphicsItem *> & rc_Items = this->items();

   // update the items
   for (QList<QGraphicsItem *>::const_iterator c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
   {
      C_GiSvDaRectBaseGroup * const pc_Item = dynamic_cast<C_GiSvDaRectBaseGroup *>(*c_ItItem);
      if (pc_Item != NULL)
      {
         pc_Item->UpdateShowValue();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle changes of transmission mode for any data element
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::UpdateTransmissionConfiguration(void) const
{
   const QList<QGraphicsItem *> & rc_Items = this->items();

   // update the items
   for (QList<QGraphicsItem *>::const_iterator c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
   {
      C_GiSvDaRectBaseGroup * const pc_Item = dynamic_cast<C_GiSvDaRectBaseGroup *>(*c_ItItem);
      if (pc_Item != NULL)
      {
         pc_Item->UpdateElementTransmissionConfiguration();
      }
   }
   //Add possible error change
   Q_EMIT (this->SigErrorChange());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle manual user operation finished event

   \param[in]  os32_Result    Operation result
   \param[in]  ou8_Nrc        Negative response code, if any
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::HandleManualOperationFinished(const int32_t os32_Result, const uint8_t ou8_Nrc) const
{
   const QList<QGraphicsItem *> & rc_Items = this->items();

   // update the items
   for (QList<QGraphicsItem *>::const_iterator c_ItItem = rc_Items.begin(); c_ItItem != rc_Items.end(); ++c_ItItem)
   {
      C_GiSvDaRectBaseGroup * const pc_Item = dynamic_cast<C_GiSvDaRectBaseGroup *>(*c_ItItem);
      if (pc_Item != NULL)
      {
         pc_Item->HandleManualOperationFinished(os32_Result, ou8_Nrc);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Trigger error check
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::TriggerErrorCheck(void) const
{
   Q_EMIT (this->SigErrorChange());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get view index

   \return
   Current view index
*/
//----------------------------------------------------------------------------------------------------------------------
uint32_t C_SyvDaDashboardScene::GetViewIndex(void) const
{
   return this->mu32_ViewIndex;
}

//----------------------------------------------------------------------------------------------------------------------
stw::opensyde_gui_logic::C_SebUnoBaseManager * C_SyvDaDashboardScene::m_GetUndoManager(void)
{
   //lint -e{1536} intentional exposure of a private member to have clean interface with inheritance in mind
   return &this->mc_UndoManager;
}

//----------------------------------------------------------------------------------------------------------------------
C_SebBaseContextMenuManager * C_SyvDaDashboardScene::m_GetContextMenuManager(void)
{
   //lint -e{1536} intentional exposure of a private member to have clean interface with inheritance in mind
   return &this->mc_ContextMenuManager;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current copy paste manager

   \return
   NULL No copy paste manager
   Else Valid copy paste manager
*/
//----------------------------------------------------------------------------------------------------------------------
stw::opensyde_gui_logic::C_SebBaseCopyPasteManager * C_SyvDaDashboardScene::m_GetCopyPasteManager(void)
{
   //lint -e{1536} intentional exposure of a private member to have clean interface with inheritance in mind
   return &this->mc_CopyPasteManager;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get current copy paste manager

   \return
   NULL No copy paste manager
   Else Valid copy paste manager
*/
//----------------------------------------------------------------------------------------------------------------------
const stw::opensyde_gui_logic::C_SebBaseCopyPasteManager * C_SyvDaDashboardScene::m_GetCopyPasteManagerConst(void) const
{
   return &this->mc_CopyPasteManager;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add image to scene

   \param[in]  orc_Path       Image path
   \param[in]  orc_Position   Image scene position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::m_AddImage(const QString & orc_Path, const QPointF & orc_Position)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      this->mc_UndoManager.DoAddGeneric(C_PuiSvDbDataElement::eIMAGE,
                                        m_GetNewUniqueId(), orc_Position, this->GetHighestUsedZetValueList(
                                           this->items()) + 1.0, pc_View->GetDarkModeActive(), orc_Path);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add data from mime data

   \param[in]  opc_MimeData   Mime data to add to scene
   \param[in]  orc_Position   Position to add data at

   \return
   true: Item was added
   false: No item was added
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaDashboardScene::m_AddOfMime(const QMimeData * const opc_MimeData, const QPointF & orc_Position)
{
   bool q_Retval = true;

   if (opc_MimeData != NULL)
   {
      C_PuiSvDbDataElement::E_Type e_Type = C_PuiSvDbDataElement::eUNKNOWN;
      QString c_Text;
      if (opc_MimeData->hasFormat("text/plain") == true)
      {
         c_Text = opc_MimeData->text();

         if (opc_MimeData->hasFormat(C_SdTopologyListWidget::hc_GROUP_NAME) == true)
         {
            const QString c_Theme1 = C_GtGetText::h_GetText("openSYDE");
            const QString c_Theme2 = C_GtGetText::h_GetText("Material");
            const QString c_Theme3 = C_GtGetText::h_GetText("Flat");
            const QString c_Theme4 = C_GtGetText::h_GetText("Skeuomorphic");
            if ((((c_Text == c_Theme1) || (c_Text == c_Theme2)) || (c_Text == c_Theme3)) || (c_Text == c_Theme4))
            {
               const QString c_Group = opc_MimeData->data(C_SdTopologyListWidget::hc_GROUP_NAME);
               if (c_Group == C_GtGetText::h_GetText("Value Label"))
               {
                  e_Type = C_PuiSvDbDataElement::eLABEL;
               }
               else if (c_Group == C_GtGetText::h_GetText("Spinbox"))
               {
                  e_Type = C_PuiSvDbDataElement::eSPIN_BOX;
               }
               else if (c_Group == C_GtGetText::h_GetText("Slider"))
               {
                  e_Type = C_PuiSvDbDataElement::eSLIDER;
               }
               else if (c_Group == C_GtGetText::h_GetText("Bar"))
               {
                  e_Type = C_PuiSvDbDataElement::ePROGRESS_BAR;
               }
               else if (c_Group == C_GtGetText::h_GetText("Toggle"))
               {
                  e_Type = C_PuiSvDbDataElement::eTOGGLE;
               }
               else if (c_Group == C_GtGetText::h_GetText("Table"))
               {
                  e_Type = C_PuiSvDbDataElement::eTABLE;
               }
               else if (c_Group == C_GtGetText::h_GetText("Pie Chart"))
               {
                  e_Type = C_PuiSvDbDataElement::ePIE_CHART;
               }
               else
               {
                  //Ignore unknown type
               }
            }
            else if (c_Text == C_GtGetText::h_GetText("Parametrization Widget"))
            {
               e_Type = C_PuiSvDbDataElement::ePARAM;
            }
            else if (c_Text == C_SebScene::mhc_BOUNDARY)
            {
               e_Type = C_PuiSvDbDataElement::eBOUNDARY;
            }
            else if (c_Text == C_SebScene::mhc_LINE)
            {
               e_Type = C_PuiSvDbDataElement::eLINE_ARROW;
            }
            else if (c_Text == C_SebScene::mhc_IMAGE)
            {
               this->m_AddImageWithFileDialog(orc_Position);
            }
            else if (c_Text != "")
            {
               // no known element, add the text as text element
               e_Type = C_PuiSvDbDataElement::eTEXT_ELEMENT;
            }
            else
            {
               //Ignore unknown type
            }
         }
         else
         {
            // check if it is an image file
            const bool q_ImageFileExist = C_SebUtil::h_CheckFilePathForImage(c_Text);

            if (q_ImageFileExist == true)
            {
               e_Type = C_PuiSvDbDataElement::eIMAGE;
            }
            else if (c_Text != "")
            {
               // no known element, add the text as text element
               e_Type = C_PuiSvDbDataElement::eTEXT_ELEMENT;
            }
            else
            {
               // nothing to do
            }
         }
      }
      else if (opc_MimeData->hasUrls() == true)
      {
         c_Text = opc_MimeData->text();

         // check if it is an image file
         const bool q_ImageFileExist = C_SebUtil::h_CheckFilePathForImage(c_Text);

         if (q_ImageFileExist == true)
         {
            e_Type = C_PuiSvDbDataElement::eIMAGE;
         }
      }
      else
      {
         q_Retval = false;
      }
      if (e_Type != C_PuiSvDbDataElement::eUNKNOWN)
      {
         const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

         if (pc_View != NULL)
         {
            this->mc_UndoManager.DoAddGeneric(e_Type, m_GetNewUniqueId(), orc_Position,
                                              this->GetHighestUsedZetValueList(this->items()) + 1.0,
                                              pc_View->GetDarkModeActive(), c_Text);
         }
      }
      else
      {
         // nothing to do
      }
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Copy items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::m_Copy(void)
{
   this->m_CopyItemsToCopyPasteManager(this->selectedItems());
   Q_EMIT (this->SigErrorChange());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Cut items
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::m_Cut(void)
{
   this->m_CopyItemsToCopyPasteManager(this->selectedItems());

   m_Delete(true);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if paste currently possible

   \return
   True  Paste possible
   False Paste impossible
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaDashboardScene::m_IsPastePossible(void)
{
   bool q_Retval = false;

   if (this->mq_EditMode == true)
   {
      q_Retval = C_SebScene::m_IsPastePossible();
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if undo currently possible

   \return
   True  Undo possible
   False Undo impossible
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaDashboardScene::m_IsUndoAvailable(void) const
{
   return this->mq_EditMode;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Optional check for user confirmation on delete action

   \param[in]  orc_SelectedItems    Selected items

   \return
   true  Continue
   false Abort
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SyvDaDashboardScene::m_HandleDeleteUserConfirmation(const QList<QGraphicsItem *> & orc_SelectedItems) const
{
   bool q_Retval = true;

   Q_UNUSED(orc_SelectedItems)

   if (this->views().size() > 0)
   {
      QGraphicsView * const pc_View = this->views().at(0);
      if (pc_View != NULL)
      {
         C_OgeWiCustomMessage::E_Outputs e_ReturnMessageBox;
         C_OgeWiCustomMessage c_MessageBox(pc_View, C_OgeWiCustomMessage::E_Type::eQUESTION);
         c_MessageBox.SetDescription(C_GtGetText::h_GetText("Do you really want to delete the selected item(s)?"));
         c_MessageBox.SetHeading(C_GtGetText::h_GetText("Items delete"));
         c_MessageBox.SetOkButtonText(C_GtGetText::h_GetText("Delete"));
         c_MessageBox.SetNoButtonText(C_GtGetText::h_GetText("Keep"));
         c_MessageBox.SetCustomMinHeight(180, 180);
         e_ReturnMessageBox = c_MessageBox.Execute();

         if (e_ReturnMessageBox == C_OgeWiCustomMessage::eOK)
         {
            q_Retval = true;
         }
         else
         {
            q_Retval = false;
         }
      }
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten key press release event slot

   \param[in,out]  opc_KeyEvent  Key event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::keyPressEvent(QKeyEvent * const opc_KeyEvent)
{
   const QList<QGraphicsItem *> & rc_Items = this->selectedItems();
   bool q_SkipSceneSpecific = false;

   if (rc_Items.size() == 1)
   {
      QGraphicsItem * const pc_Item = rc_Items.at(0);
      QGraphicsItem * const pc_Parent = C_SebUtil::h_GetHighestParent(pc_Item);
      C_GiSvDaRectBaseGroup * const pc_Widget = dynamic_cast<C_GiSvDaRectBaseGroup *>(pc_Parent);

      if (pc_Widget != NULL)
      {
         if (pc_Widget->IsEditContentEnabled() == true)
         {
            // In case of edit content mode, all key press events will be handled of the "inner" widgets
            // For achieving this skipping of the scene specific functionality and forward the event
            // to the original keyPressEvent which will forward the signal to the selected element
            q_SkipSceneSpecific = true;
         }
         else if ((opc_KeyEvent->key() == static_cast<int32_t>(Qt::Key_F2)) &&
                  (this->mq_EditMode == true))
         {
            pc_Widget->EnableEditContent();
            opc_KeyEvent->accept();
         }
         else
         {
            // Nothing to do
         }
      }
   }

   if (q_SkipSceneSpecific == false)
   {
      C_SebScene::keyPressEvent(opc_KeyEvent);
   }
   else
   {
      QGraphicsScene::keyPressEvent(opc_KeyEvent);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten context menu event

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::contextMenuEvent(QGraphicsSceneContextMenuEvent * const opc_Event)
{
   // Prepare the widgets for showing the context menu item
   const QList<QGraphicsItem *> & rc_AllItems = this->items();
   const QList<QGraphicsItem *> & rc_Items = this->selectedItems();

   // Deactivate all other old connections to the actions first
   // We have to do it here, because the action signal comes always after all close, hide etc. signals of the context
   // menu itself. We can not disconnect without preventing to get the action signal once.
   for (QList<QGraphicsItem *>::const_iterator c_ItItem = rc_AllItems.begin(); c_ItItem != rc_AllItems.end();
        ++c_ItItem)
   {
      C_GiSvDaRectBaseGroup * const pc_Widget = dynamic_cast<C_GiSvDaRectBaseGroup *>(*c_ItItem);
      if (pc_Widget != NULL)
      {
         pc_Widget->ConfigureContextMenu(&this->mc_ContextMenuManager, false);
      }
   }

   if ((rc_Items.size() > 0L) || (this->mq_EditMode == true))
   {
      // The specific functions for a dashboard widget are only available if only one item is selected.
      if (rc_Items.size() == 1)
      {
         // inform the item
         QGraphicsItem * const pc_Item = rc_Items.at(0);
         QGraphicsItem * const pc_Parent = C_SebUtil::h_GetHighestParent(pc_Item);

         C_GiSvDaRectBaseGroup * const pc_Widget = dynamic_cast<C_GiSvDaRectBaseGroup *>(pc_Parent);
         if (pc_Widget != NULL)
         {
            pc_Widget->ConfigureContextMenu(&this->mc_ContextMenuManager, true);
         }
      }

      C_SebScene::contextMenuEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten mouse press event slot

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   bool q_CallSebScene = true;

   // Avoid in case of enabled content edit mode changing the selection or other interactions of C_SebScene
   // with the right mouse click. The event must be handled by the proxy widget of the dashboard widget in this
   // scenario
   if ((this->mq_EditMode == true) &&
       (opc_Event->button() == Qt::RightButton))
   {
      const QList<QGraphicsItem *> & rc_Items = this->selectedItems();

      if (rc_Items.size() == 1)
      {
         QGraphicsItem * const pc_Item = rc_Items.at(0);
         QGraphicsItem * const pc_Parent = C_SebUtil::h_GetHighestParent(pc_Item);
         C_GiSvDaRectBaseGroup * const pc_Widget = dynamic_cast<C_GiSvDaRectBaseGroup *>(pc_Parent);

         if ((pc_Widget != NULL) &&
             (pc_Widget->IsEditContentEnabled() == true) &&
             (pc_Widget->IsMousePosRelevantForProxyWidgetInteraction(opc_Event->scenePos())))
         {
            q_CallSebScene = false;
         }
      }
   }

   if (q_CallSebScene == true)
   {
      C_SebScene::mousePressEvent(opc_Event);
   }
   else
   {
      QGraphicsScene::mousePressEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse double click event slot

   \param[in,out]  opc_Event  Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   const QList<QGraphicsItem *> & rc_SelectedItems = this->selectedItems();

   if (rc_SelectedItems.count() == 1)
   {
      C_GiSvDaRectBaseGroup * const pc_RectBase = dynamic_cast<C_GiSvDaRectBaseGroup * const>(rc_SelectedItems[0]);

      if (pc_RectBase != NULL)
      {
         if (pc_RectBase->IsEditContentEnabled() == true)
         {
            // In case of an already activated edit content mode forward the event
            C_SebScene::mouseDoubleClickEvent(opc_Event);
         }
         // Check for supporting properties dialog
         else if (pc_RectBase->CallProperties() == false)
         {
            // Next try for enabling edit content mode
            if (pc_RectBase->EnableEditContent() == false)
            {
               C_SebScene::mouseDoubleClickEvent(opc_Event);
            }
         }
         else
         {
            // Nothing to do
         }
      }
      else
      {
         C_SebScene::mouseDoubleClickEvent(opc_Event);
      }
   }
   else
   {
      C_SebScene::mouseDoubleClickEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle edit properties

   \param[in,out]  opc_Item   Item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::m_OnWidgetEditProperties(QGraphicsItem * const opc_Item) const
{
   C_GiSvDaRectBaseGroup * const pc_RectBase = dynamic_cast<C_GiSvDaRectBaseGroup * const>(opc_Item);

   if (pc_RectBase != NULL)
   {
      pc_RectBase->CallProperties();
      Q_EMIT (this->SigErrorChange());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle edit content

   \param[in,out]  opc_Item   Item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::m_OnWidgetEditContent(QGraphicsItem * const opc_Item) const
{
   C_GiSvDaRectBaseGroup * const pc_RectBase = dynamic_cast<C_GiSvDaRectBaseGroup * const>(opc_Item);

   if (pc_RectBase != NULL)
   {
      pc_RectBase->EnableEditContent();
      Q_EMIT (this->SigErrorChange());
   }
   else
   {
      // Special case: Text element
      C_GiSvDaTextElement  * const pc_TextElement = dynamic_cast<C_GiSvDaTextElement * const>(opc_Item);

      if (pc_TextElement != NULL)
      {
         pc_TextElement->EnableEditContent();
         Q_EMIT (this->SigErrorChange());
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add param widget to scene and connnect signals

   \param[in,out]  opc_Item   Param widget item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::m_AddParamWidgetToScene(C_GiSvDaParam * const opc_Item)
{
   m_AddWidgetToScene(opc_Item);
   connect(opc_Item, &C_GiSvDaParam::SigNvmReadList, this, &C_SyvDaDashboardScene::SigNvmReadList);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Add widget to scene and connnect signals

   \param[in,out]  opc_Item   Widget item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::m_AddWidgetToScene(C_GiSvDaRectBaseGroup * const opc_Item)
{
   //Connection(s)
   connect(opc_Item, &C_GiSvDaRectBaseGroup::SigWidgetHandling, this,
           &C_SyvDaDashboardScene::m_HandleProxyWidgetInteractionChange);
   connect(opc_Item, &C_GiSvDaRectBaseGroup::SigSelected, this,
           &C_SyvDaDashboardScene::m_UpdateSelection);
   connect(opc_Item, &C_GiSvDaRectBaseGroup::SigDataPoolWrite, this,
           &C_SyvDaDashboardScene::SigDataPoolWrite);
   connect(opc_Item, &C_GiSvDaRectBaseGroup::SigDataPoolRead, this,
           &C_SyvDaDashboardScene::SigDataPoolRead);
   connect(opc_Item, &C_GiSvDaRectBaseGroup::SigTriggerUpdateTransmissionConfiguration, this,
           &C_SyvDaDashboardScene::SigTriggerUpdateTransmissionConfiguration);
   connect(opc_Item, &C_GiSvDaRectBaseGroup::SigDataElementsChanged, this,
           &C_SyvDaDashboardScene::SigErrorChange);
   connect(opc_Item, &C_GiSvDaRectBaseGroup::SigHideToolTip, this,
           &C_SyvDaDashboardScene::m_HandleHideToolTip);

   opc_Item->EditModeActiveChanged(this->mq_EditMode);

   m_AddRectBaseGroupToScene(opc_Item);
   m_AddAnyItemToScene(opc_Item);
   Q_EMIT (this->SigErrorChange());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete widget of scene and disconnect signals

   \param[in,out]  opc_Item   Widget item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::m_RemoveWidgetOfScene(const C_GiSvDaRectBaseGroup * const opc_Item)
{
   if (opc_Item != NULL)
   {
      //Connection(s)
      disconnect(opc_Item, &C_GiSvDaRectBaseGroup::SigWidgetHandling, this,
                 &C_SyvDaDashboardScene::m_HandleProxyWidgetInteractionChange);
      disconnect(opc_Item, &C_GiSvDaRectBaseGroup::SigSelected, this,
                 &C_SyvDaDashboardScene::m_UpdateSelection);
      disconnect(opc_Item, &C_GiSvDaRectBaseGroup::SigDataPoolWrite, this,
                 &C_SyvDaDashboardScene::SigDataPoolWrite);
      disconnect(opc_Item, &C_GiSvDaRectBaseGroup::SigDataPoolRead, this,
                 &C_SyvDaDashboardScene::SigDataPoolRead);
      disconnect(opc_Item, &C_GiSvDaRectBaseGroup::SigTriggerUpdateTransmissionConfiguration, this,
                 &C_SyvDaDashboardScene::SigTriggerUpdateTransmissionConfiguration);
      disconnect(opc_Item, &C_GiSvDaRectBaseGroup::SigDataElementsChanged, this,
                 &C_SyvDaDashboardScene::SigErrorChange);
      disconnect(opc_Item, &C_GiSvDaRectBaseGroup::SigHideToolTip, this,
                 &C_SyvDaDashboardScene::m_HandleHideToolTip);

      m_RemoveRectBaseGroupOfScene(opc_Item);
      //Update indices
      this->m_SyncIndex(opc_Item->GetWidgetType(), opc_Item->GetIndex(), C_PuiSvDbDataElement::eDELETE);
      Q_EMIT (this->SigErrorChange());
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete boundary of scene and disconnect signals

   \param[in,out]  opc_Item   Boundary item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::m_RemoveBoundaryOfScene(const C_GiSvDaBoundary * const opc_Item)
{
   if (opc_Item != NULL)
   {
      m_RemoveRectBaseGroupOfScene(opc_Item);
      //Update indices
      this->m_SyncIndex(C_PuiSvDbDataElement::eBOUNDARY, opc_Item->GetIndex(),
                        C_PuiSdDataElement::eDELETE);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete text element of scene and disconnect signals

   \param[in,out]  opc_Item   Text element item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::m_RemoveTextElementOfScene(const C_GiSvDaTextElement * const opc_Item)
{
   if (opc_Item != NULL)
   {
      disconnect(opc_Item, &C_GiBiTextElement::SigTextInteractionModeStateChanged, this,
                 &C_SyvDaDashboardScene::m_HandleProxyWidgetInteractionChange);
      m_RemoveRectBaseGroupOfScene(opc_Item);
      //Update indices
      this->m_SyncIndex(C_PuiSvDbDataElement::eTEXT_ELEMENT, opc_Item->GetIndex(),
                        C_PuiSdDataElement::eDELETE);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete image group of scene and disconnect signals

   \param[in,out]  opc_Item   Image group item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::m_RemoveImageGroupOfScene(const C_GiSvDaImageGroup * const opc_Item)
{
   if (opc_Item != NULL)
   {
      m_RemoveRectBaseGroupOfScene(opc_Item);
      //Update indices
      this->m_SyncIndex(C_PuiSvDbDataElement::eIMAGE, opc_Item->GetIndex(),
                        C_PuiSdDataElement::eDELETE);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Delete line arrow of scene and disconnect signals

   \param[in,out]  opc_Item   Line arrow item
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::m_RemoveLineArrowOfScene(const C_GiSvDaArrow * const opc_Item)
{
   if (opc_Item != NULL)
   {
      m_RemoveLineGroupOfScene(opc_Item);
      //Update indices
      this->m_SyncIndex(C_PuiSvDbDataElement::eLINE_ARROW, opc_Item->GetIndex(),
                        C_PuiSdDataElement::eDELETE);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Adapt index if item in array was deleted

   \param[in]  ore_Type       Type of vector which changed
   \param[in]  ors32_Index    Index of vector which changed
   \param[in]  ore_Action     Type of action
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::m_SyncIndex(const C_PuiSvDbDataElement::E_Type & ore_Type, const int32_t & ors32_Index,
                                        const C_PuiBsDataElement::E_Action & ore_Action) const
{
   QList<QGraphicsItem *> c_Items = this->items();

   //Fill
   for (QList<QGraphicsItem *>::iterator c_ItItem = c_Items.begin(); c_ItItem != c_Items.end();
        ++c_ItItem)
   {
      // TODO Watch if parent necessary

      C_PuiSvDbDataElement * const pc_DataElement = dynamic_cast<C_PuiSvDbDataElement *>(*c_ItItem);
      if (pc_DataElement != NULL)
      {
         pc_DataElement->SyncIndex(ore_Type, ors32_Index, ore_Action);
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Load subset of system definition entries

   \param[in]  orc_OtherStartIndices   Start indices
                                       0: Boundary
                                       1: Text element
                                       2: Line arrow
                                       3: Image
                                       4: Label
                                       5: Spin box
                                       6: Slider
                                       7: Progress bar
                                       8: Toggle
                                       9: Pie chart
                                       10: Table
                                       11: Param
   \param[in]  orq_Selection           False: Ignore selection
   \param[in]  opc_IdMap               Optional map for IDs to use
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::m_LoadSubset(const QVector<uint32_t> & orc_OtherStartIndices, const bool & orq_Selection,
                                         const QMap<C_PuiBsTemporaryDataId, uint64_t> * const opc_IdMap)
{
   const C_PuiSvData * const pc_View = C_PuiSvHandler::h_GetInstance()->GetView(this->mu32_ViewIndex);

   if (pc_View != NULL)
   {
      const C_PuiSvDashboard * const pc_Dashboard = pc_View->GetDashboard(this->mu32_DashboardIndex);

      if (pc_Dashboard != NULL)
      {
         //Load
         uint64_t u64_CurUniqueId;
         const std::vector<C_PuiSvDbLabel> & rc_Labels = pc_Dashboard->GetLabels();
         const std::vector<C_PuiSvDbParam> & rc_Params = pc_Dashboard->GetParams();
         const std::vector<C_PuiSvDbPieChart> & rc_PieCharts = pc_Dashboard->GetPieCharts();
         const std::vector<C_PuiSvDbSpinBox> & rc_SpinBoxes = pc_Dashboard->GetSpinBoxes();
         const std::vector<C_PuiSvDbSlider> & rc_Sliders = pc_Dashboard->GetSliders();
         const std::vector<C_PuiSvDbProgressBar> & rc_ProgressBars = pc_Dashboard->GetProgressBars();
         const std::vector<C_PuiSvDbTable> & rc_Tables = pc_Dashboard->GetTables();
         const std::vector<C_PuiSvDbToggle> & rc_Toggles = pc_Dashboard->GetToggles();

         if (orq_Selection == true)
         {
            this->clearSelection();
         }

         //Create
         //Labels
         for (uint32_t u32_ItWidget = orc_OtherStartIndices[4]; u32_ItWidget < rc_Labels.size(); ++u32_ItWidget)
         {
            const C_PuiSvDbWidgetBase & rc_WidgetBase = rc_Labels[u32_ItWidget];
            C_GiSvDaRectBaseGroup * pc_Item;
            //ID
            u64_CurUniqueId = m_GetNewUniqueId(opc_IdMap, static_cast<int32_t>(C_PuiSvDbDataElement::eLABEL),
                                               u32_ItWidget - orc_OtherStartIndices[4]);

            pc_Item = new C_GiSvDaLabelBase(this->mu32_ViewIndex,
                                            this->mu32_DashboardIndex, static_cast<int32_t>(u32_ItWidget),
                                            u64_CurUniqueId);
            pc_Item->LoadData();
            pc_Item->SetDisplayStyle(rc_WidgetBase.e_DisplayStyle, pc_View->GetDarkModeActive());

            m_AddWidgetToScene(pc_Item);
            if (orq_Selection == true)
            {
               m_UpdateSelection(pc_Item, false);
            }
         }

         //Param widgets
         for (uint32_t u32_ItWidget = orc_OtherStartIndices[11]; u32_ItWidget < rc_Params.size(); ++u32_ItWidget)
         {
            const C_PuiSvDbWidgetBase & rc_WidgetBase = rc_Params[u32_ItWidget];
            C_GiSvDaParam * pc_Item;
            //ID
            u64_CurUniqueId = m_GetNewUniqueId(opc_IdMap, static_cast<int32_t>(C_PuiSvDbDataElement::ePARAM),
                                               u32_ItWidget - orc_OtherStartIndices[11]);

            pc_Item = new C_GiSvDaParam(this->mu32_ViewIndex, this->mu32_DashboardIndex,
                                        static_cast<int32_t>(u32_ItWidget), u64_CurUniqueId);
            pc_Item->LoadData();
            pc_Item->SetDisplayStyle(rc_WidgetBase.e_DisplayStyle, pc_View->GetDarkModeActive());

            m_AddParamWidgetToScene(pc_Item);
            if (orq_Selection == true)
            {
               m_UpdateSelection(pc_Item, false);
            }
         }

         //Pie charts
         for (uint32_t u32_ItWidget = orc_OtherStartIndices[9]; u32_ItWidget < rc_PieCharts.size(); ++u32_ItWidget)
         {
            const C_PuiSvDbWidgetBase & rc_WidgetBase = rc_PieCharts[u32_ItWidget];
            C_GiSvDaRectBaseGroup * pc_Item;
            //ID
            u64_CurUniqueId = m_GetNewUniqueId(opc_IdMap, static_cast<int32_t>(C_PuiSvDbDataElement::ePIE_CHART),
                                               u32_ItWidget - orc_OtherStartIndices[9]);

            pc_Item = new C_GiSvDaPieChartBase(this->mu32_ViewIndex,
                                               this->mu32_DashboardIndex, static_cast<int32_t>(u32_ItWidget),
                                               u64_CurUniqueId);
            pc_Item->LoadData();
            pc_Item->SetDisplayStyle(rc_WidgetBase.e_DisplayStyle, pc_View->GetDarkModeActive());

            m_AddWidgetToScene(pc_Item);
            if (orq_Selection == true)
            {
               m_UpdateSelection(pc_Item, false);
            }
         }

         //Spin boxes
         for (uint32_t u32_ItWidget = orc_OtherStartIndices[5]; u32_ItWidget < rc_SpinBoxes.size(); ++u32_ItWidget)
         {
            const C_PuiSvDbWidgetBase & rc_WidgetBase = rc_SpinBoxes[u32_ItWidget];
            C_GiSvDaRectBaseGroup * pc_Item;
            //ID
            u64_CurUniqueId = m_GetNewUniqueId(opc_IdMap, static_cast<int32_t>(C_PuiSvDbDataElement::eSPIN_BOX),
                                               u32_ItWidget - orc_OtherStartIndices[5]);

            pc_Item = new C_GiSvDaSpinBoxBase(this->mu32_ViewIndex,
                                              this->mu32_DashboardIndex, static_cast<int32_t>(u32_ItWidget),
                                              u64_CurUniqueId);
            pc_Item->LoadData();
            pc_Item->SetDisplayStyle(rc_WidgetBase.e_DisplayStyle, pc_View->GetDarkModeActive());

            m_AddWidgetToScene(pc_Item);
            if (orq_Selection == true)
            {
               m_UpdateSelection(pc_Item, false);
            }
         }

         //Sliders
         for (uint32_t u32_ItWidget = orc_OtherStartIndices[6]; u32_ItWidget < rc_Sliders.size(); ++u32_ItWidget)
         {
            const C_PuiSvDbWidgetBase & rc_WidgetBase = rc_Sliders[u32_ItWidget];
            C_GiSvDaRectBaseGroup * pc_Item;
            //ID
            u64_CurUniqueId = m_GetNewUniqueId(opc_IdMap, static_cast<int32_t>(C_PuiSvDbDataElement::eSLIDER),
                                               u32_ItWidget - orc_OtherStartIndices[6]);

            pc_Item = new C_GiSvDaSliderBase(this->mu32_ViewIndex,
                                             this->mu32_DashboardIndex, static_cast<int32_t>(u32_ItWidget),
                                             u64_CurUniqueId);
            pc_Item->LoadData();
            pc_Item->SetDisplayStyle(rc_WidgetBase.e_DisplayStyle, pc_View->GetDarkModeActive());

            m_AddWidgetToScene(pc_Item);
            if (orq_Selection == true)
            {
               m_UpdateSelection(pc_Item, false);
            }
         }

         //Tables
         for (uint32_t u32_ItWidget = orc_OtherStartIndices[10]; u32_ItWidget < rc_Tables.size(); ++u32_ItWidget)
         {
            const C_PuiSvDbWidgetBase & rc_WidgetBase = rc_Tables[u32_ItWidget];
            C_GiSvDaRectBaseGroup * pc_Item;
            //ID
            u64_CurUniqueId = m_GetNewUniqueId(opc_IdMap, static_cast<int32_t>(C_PuiSvDbDataElement::eTABLE),
                                               u32_ItWidget - orc_OtherStartIndices[10]);

            pc_Item = new C_GiSvDaTableBase(this->mu32_ViewIndex,
                                            this->mu32_DashboardIndex, static_cast<int32_t>(u32_ItWidget),
                                            u64_CurUniqueId);
            pc_Item->LoadData();
            pc_Item->SetDisplayStyle(rc_WidgetBase.e_DisplayStyle, pc_View->GetDarkModeActive());

            m_AddWidgetToScene(pc_Item);
            if (orq_Selection == true)
            {
               m_UpdateSelection(pc_Item, false);
            }
         }

         //Progress bars
         for (uint32_t u32_ItWidget = orc_OtherStartIndices[7]; u32_ItWidget < rc_ProgressBars.size(); ++u32_ItWidget)
         {
            const C_PuiSvDbWidgetBase & rc_WidgetBase = rc_ProgressBars[u32_ItWidget];
            C_GiSvDaRectBaseGroup * pc_Item;
            //ID
            u64_CurUniqueId = m_GetNewUniqueId(opc_IdMap, static_cast<int32_t>(C_PuiSvDbDataElement::ePROGRESS_BAR),
                                               u32_ItWidget - orc_OtherStartIndices[7]);

            pc_Item = new C_GiSvDaProgressBarBase(this->mu32_ViewIndex,
                                                  this->mu32_DashboardIndex, static_cast<int32_t>(u32_ItWidget),
                                                  u64_CurUniqueId);
            pc_Item->LoadData();
            pc_Item->SetDisplayStyle(rc_WidgetBase.e_DisplayStyle, pc_View->GetDarkModeActive());

            m_AddWidgetToScene(pc_Item);
            if (orq_Selection == true)
            {
               m_UpdateSelection(pc_Item, false);
            }
         }

         //Toggles
         for (uint32_t u32_ItWidget = orc_OtherStartIndices[8]; u32_ItWidget < rc_Toggles.size(); ++u32_ItWidget)
         {
            const C_PuiSvDbWidgetBase & rc_WidgetBase = rc_Toggles[u32_ItWidget];
            C_GiSvDaRectBaseGroup * pc_Item;
            //ID
            u64_CurUniqueId = m_GetNewUniqueId(opc_IdMap, static_cast<int32_t>(C_PuiSvDbDataElement::eTOGGLE),
                                               u32_ItWidget - orc_OtherStartIndices[8]);

            pc_Item = new C_GiSvDaToggleBase(this->mu32_ViewIndex,
                                             this->mu32_DashboardIndex, static_cast<int32_t>(u32_ItWidget),
                                             u64_CurUniqueId);
            pc_Item->LoadData();
            pc_Item->SetDisplayStyle(rc_WidgetBase.e_DisplayStyle, pc_View->GetDarkModeActive());

            m_AddWidgetToScene(pc_Item);
            if (orq_Selection == true)
            {
               m_UpdateSelection(pc_Item, false);
            }
         }

         // Boundary
         for (uint32_t u32_Item = orc_OtherStartIndices[0];
              u32_Item < pc_Dashboard->c_Boundaries.size();
              ++u32_Item)
         {
            const C_PuiBsBoundary * const pc_UiBoundaryData = &pc_Dashboard->c_Boundaries[u32_Item];
            C_GiSvDaBoundary * pc_Item;

            //ID
            u64_CurUniqueId = m_GetNewUniqueId(opc_IdMap, static_cast<int32_t>(C_PuiSvDbDataElement::eBOUNDARY),
                                               u32_Item - orc_OtherStartIndices[0]);

            pc_Item = new C_GiSvDaBoundary(this->mu32_ViewIndex, this->mu32_DashboardIndex, u32_Item, u64_CurUniqueId,
                                           pc_UiBoundaryData->f64_Width, pc_UiBoundaryData->f64_Height);
            pc_Item->LoadData();

            this->m_AddBoundaryToScene(pc_Item);
            if (orq_Selection == true)
            {
               m_UpdateSelection(pc_Item, false);
            }
         }

         // Text Element
         for (uint32_t u32_Item = orc_OtherStartIndices[1];
              u32_Item < pc_Dashboard->c_TextElements.size();
              ++u32_Item)
         {
            C_GiSvDaTextElement * pc_Item;

            //ID
            u64_CurUniqueId = m_GetNewUniqueId(opc_IdMap, static_cast<int32_t>(C_PuiSvDbDataElement::eTEXT_ELEMENT),
                                               u32_Item - orc_OtherStartIndices[1]);

            pc_Item =
               new C_GiSvDaTextElement(this->mu32_ViewIndex, this->mu32_DashboardIndex, u32_Item, u64_CurUniqueId);
            pc_Item->LoadData();

            this->m_AddTextElementToScene(pc_Item);

            connect(pc_Item, &C_GiBiTextElement::SigTextInteractionModeStateChanged, this,
                    &C_SyvDaDashboardScene::m_HandleProxyWidgetInteractionChange);

            if (orq_Selection == true)
            {
               m_UpdateSelection(pc_Item, false);
            }
         }

         // Line arrow
         for (uint32_t u32_Item = orc_OtherStartIndices[2];
              u32_Item < pc_Dashboard->c_LineArrows.size();
              ++u32_Item)
         {
            C_GiSvDaArrow * pc_Item;

            //ID
            u64_CurUniqueId = m_GetNewUniqueId(opc_IdMap, static_cast<int32_t>(C_PuiSvDbDataElement::eLINE_ARROW),
                                               u32_Item - orc_OtherStartIndices[2]);

            pc_Item = new C_GiSvDaArrow(this->mu32_ViewIndex, this->mu32_DashboardIndex, u32_Item, u64_CurUniqueId);
            this->m_AddLineArrowToScene(pc_Item);
            if (orq_Selection == true)
            {
               m_UpdateSelection(pc_Item, false);
            }
         }

         // Image
         for (uint32_t u32_Item = orc_OtherStartIndices[3];
              u32_Item < pc_Dashboard->c_Images.size();
              ++u32_Item)
         {
            const C_PuiBsImage * const pc_UiImageData = &pc_Dashboard->c_Images[u32_Item];
            C_GiSvDaImageGroup * pc_Item;

            //ID
            u64_CurUniqueId = m_GetNewUniqueId(opc_IdMap, static_cast<int32_t>(C_PuiSvDbDataElement::eIMAGE),
                                               u32_Item - orc_OtherStartIndices[3]);

            pc_Item = new C_GiSvDaImageGroup(this->mu32_ViewIndex, this->mu32_DashboardIndex, u32_Item,
                                             u64_CurUniqueId, pc_UiImageData->f64_Width,
                                             pc_UiImageData->f64_Height, pc_UiImageData->c_UiImagePixmap);
            pc_Item->LoadData();

            this->m_AddImageGroupToScene(pc_Item);
            if (orq_Selection == true)
            {
               m_UpdateSelection(pc_Item, false);
            }
         }

         //Clean selections of constructor
         if (orq_Selection == false)
         {
            this->clearSelection();
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Handle selection change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::m_SelectionChanged(void)
{
   QList<QGraphicsItem *>::const_iterator c_ItItem;
   QList<QGraphicsItem *> c_SelectedItems = this->selectedItems();

   // triggered by signal selectionChanged
   if (c_SelectedItems.size() == 1)
   {
      C_GiBiRectBaseGroup * const pc_Item = dynamic_cast<C_GiBiRectBaseGroup *>(c_SelectedItems[0]);

      // check if the only one selected item is a resizable rectangle based item
      if (pc_Item != NULL)
      {
         // activate the resizing only if exactly one node is selected
         pc_Item->SetResizing(true);
      }
      else
      {
         C_GiLiLineGroup * const pc_LineItem = dynamic_cast<C_GiLiLineGroup *>(c_SelectedItems[0]);
         if (pc_LineItem != NULL)
         {
            //Custom rubberband flag
            if (this->m_IsRubberBandActive() == true)
            {
               pc_LineItem->SetResizing(false);
            }
            else
            {
               pc_LineItem->SetResizing(true);
            }
         }
      }
   }
   else if (this->mq_ProxyWidgetInteractionActive == false)
   {
      // deactivate resizing for all nodes
      for (c_ItItem = c_SelectedItems.begin(); c_ItItem != c_SelectedItems.end(); ++c_ItItem)
      {
         // check if the only one selected item is a resizable rectangle based item

         C_GiBiRectBaseGroup * const pc_Item = dynamic_cast<C_GiBiRectBaseGroup *>(*c_ItItem);
         if (pc_Item != NULL)
         {
            pc_Item->SetResizing(false);

            C_GiSvDaRectBaseGroup * const pc_DbItem = dynamic_cast<C_GiSvDaRectBaseGroup *>(pc_Item);
            if ((pc_DbItem != NULL) && (pc_DbItem->IsEditContentEnabled() == true))
            {
               // In case of a change of number of selected items from 1 to n and a still active edit content mode
               // the mode must be disabled
               pc_DbItem->DisableEditContent();
            }
         }
         else
         {
            C_GiLiLineGroup * const pc_LineItem = dynamic_cast<C_GiLiLineGroup *>(*c_ItItem);
            if (pc_LineItem != NULL)
            {
               pc_LineItem->SetResizing(false);
            }
         }
      }
   }
   else
   {
      this->clearSelection();
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Initialize base context menu manager signals (AFTER virtual get context menu manager is valid
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SyvDaDashboardScene::m_InitBaseSceneContextMenuManager(void) const
{
   connect(&this->mc_ContextMenuManager, &C_SyvDaContextMenuManager::SigEditProperties, this,
           &C_SyvDaDashboardScene::m_OnWidgetEditProperties);
   connect(&this->mc_ContextMenuManager, &C_SyvDaContextMenuManager::SigEditContent, this,
           &C_SyvDaDashboardScene::m_OnWidgetEditContent);
}
