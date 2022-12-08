//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Data pool list array edit view (implementation)

   Data pool list array edit view

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QScrollBar>
#include <QHeaderView>
#include <QMimeData>
#include <QDrag>
#include "C_SdNdeDpListArrayEditView.hpp"
#include "C_PuiSdHandler.hpp"
#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "C_Uti.hpp"
#include "C_SdNdeDpUtil.hpp"
#include "C_SdNdeSingleHeaderView.hpp"
#include "C_GtGetText.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;
using namespace stw::opensyde_core;
using namespace stw::opensyde_gui_elements;
using namespace stw::errors;
using namespace stw::tgl;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeDpListArrayEditView::C_SdNdeDpListArrayEditView(QWidget * const opc_Parent) :
   C_TblViewScroll(opc_Parent),
   mu32_NodeIndex(0),
   mu32_DataPoolIndex(0),
   mu32_ListIndex(0),
   mu32_ElementIndex(0),
   me_ArrayEditType(C_SdNdeDpUtil::eARRAY_EDIT_MIN),
   mu32_DataSetIndex(0),
   mc_UndoManager(),
   mc_Delegate(),
   mpc_ModelViewManager(NULL),
   mpc_LabelCorner(NULL)
{
   //UI Settings
   this->setSortingEnabled(false);
   this->setGridStyle(Qt::NoPen);
   this->setShowGrid(false);
   this->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectColumns);
   this->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
   this->setAlternatingRowColors(true);
   this->setDragDropMode(QAbstractItemView::NoDragDrop);
   this->setDefaultDropAction(Qt::DropAction::MoveAction);
   this->setDragEnabled(false);
   this->setLineWidth(0);
   this->setFrameShadow(QAbstractItemView::Shadow::Plain);
   this->setFrameShape(QAbstractItemView::Shape::NoFrame);
   this->setEditTriggers(
      QAbstractItemView::DoubleClicked /* | QAbstractItemView::CurrentChanged*/ | QAbstractItemView::AnyKeyPressed |
      QAbstractItemView::EditKeyPressed);
   //Consider all elements for resize
   this->setVerticalHeader(new C_SdNdeSingleHeaderView(Qt::Vertical));
   this->verticalHeader()->setResizeContentsPrecision(-1);
   this->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
   this->verticalHeader()->setFixedHeight(30);
   //Row Height
   this->setHorizontalHeader(new C_SdNdeSingleHeaderView(Qt::Horizontal));
   this->horizontalHeader()->setResizeContentsPrecision(-1);
   this->horizontalHeader()->setDefaultSectionSize(70);
   this->horizontalHeader()->setFixedHeight(30);
   //Corner button
   this->setCornerButtonEnabled(false);

   this->setItemDelegate(&mc_Delegate);

   //Hover event
   this->setMouseTracking(true);

   //Corner button label
   {
      this->mpc_LabelCorner = new QLabel(this);
      this->mpc_LabelCorner->setAlignment(Qt::AlignCenter);
      this->mpc_LabelCorner->setAttribute(Qt::WA_TransparentForMouseEvents);
      this->mpc_LabelCorner->setText(C_GtGetText::h_GetText("Index"));

      connect(
         this->verticalHeader(), &QHeaderView::geometriesChanged, this,
         &C_SdNdeDpListArrayEditView::m_UpdateCornerButton);
      connect(
         this->horizontalHeader(), &QHeaderView::geometriesChanged, this,
         &C_SdNdeDpListArrayEditView::m_UpdateCornerButton);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent and the Qt memory management or never took ownership
C_SdNdeDpListArrayEditView::~C_SdNdeDpListArrayEditView(void)
{
   if (this->mpc_ModelViewManager != NULL)
   {
      this->mpc_ModelViewManager->UnRegisterArrayEditView(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                          this->mu32_ListIndex, this->mu32_ElementIndex,
                                                          this->me_ArrayEditType, this->mu32_DataSetIndex, this);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Specify associated list

   \param[in] oru32_NodeIndex     Node index
   \param[in] oru32_DataPoolIndex Node data pool index
   \param[in] oru32_ListIndex     Node data pool list index
   \param[in] oru32_ElementIndex  Node data pool list element index
   \param[in] ore_ArrayEditType   Enum for node data pool list element variable
   \param[in] oru32_DataSetIndex  If min or max use 0
                                  Else use data set index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListArrayEditView::SetElement(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                            const uint32_t & oru32_ListIndex, const uint32_t & oru32_ElementIndex,
                                            const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                                            const uint32_t & oru32_DataSetIndex)
{
   if (this->mpc_ModelViewManager != NULL)
   {
      this->mpc_ModelViewManager->UnRegisterArrayEditView(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                          this->mu32_ListIndex, this->mu32_ElementIndex,
                                                          this->me_ArrayEditType, this->mu32_DataSetIndex, this);
   }
   this->mu32_NodeIndex = oru32_NodeIndex;
   this->mu32_DataPoolIndex = oru32_DataPoolIndex;
   this->mu32_ListIndex = oru32_ListIndex;
   this->mu32_ElementIndex = oru32_ElementIndex;
   this->me_ArrayEditType = ore_ArrayEditType;
   this->mu32_DataSetIndex = oru32_DataSetIndex;
   m_UpdateModelView();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Check if equal

   \param[in] oru32_NodeIndex     Node index
   \param[in] oru32_DataPoolIndex Node data pool index
   \param[in] oru32_ListIndex     Node data pool list index
   \param[in] oru32_ElementIndex  Node data pool list element index
   \param[in] ore_ArrayEditType   Enum for node data pool list element variable
   \param[in] oru32_DataSetIndex  If min or max use 0
                                  Else use data set index

   \return
   True  Match
   False No match
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdNdeDpListArrayEditView::Equals(const uint32_t & oru32_NodeIndex, const uint32_t & oru32_DataPoolIndex,
                                        const uint32_t & oru32_ListIndex, const uint32_t & oru32_ElementIndex,
                                        const C_SdNdeDpUtil::E_ArrayEditType & ore_ArrayEditType,
                                        const uint32_t & oru32_DataSetIndex) const
{
   bool q_Retval;

   if ((((((this->mu32_NodeIndex == oru32_NodeIndex) && (this->mu32_DataPoolIndex == oru32_DataPoolIndex)) &&
          (this->mu32_ListIndex == oru32_ListIndex)) && (this->mu32_ElementIndex == oru32_ElementIndex)) &&
        (this->me_ArrayEditType == ore_ArrayEditType)) && (this->mu32_DataSetIndex == oru32_DataSetIndex))
   {
      q_Retval = true;
   }
   else
   {
      q_Retval = false;
   }
   return q_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   GetUndocommand and take ownership

   Internal undo command is reseted

   \return
   NULL No changes
   Else Undocommand accumulating all changes
*/
//----------------------------------------------------------------------------------------------------------------------
QUndoCommand * C_SdNdeDpListArrayEditView::TakeUndoCommand(void)
{
   return this->mc_UndoManager.TakeUndoCommand();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Forward signal
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListArrayEditView::OnErrorChangePossible(void)
{
   Q_EMIT this->SigErrorChangePossible();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Do data change

   \param[in] oru32_ArrayIndex Array index
   \param[in] orc_NewData      New data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListArrayEditView::OnDataChange(const uint32_t & oru32_ArrayIndex, const QVariant & orc_NewData)
{
   this->mc_UndoManager.DoDataChangeElements(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                             this->mu32_ListIndex, this->mu32_ElementIndex, this->me_ArrayEditType,
                                             this->mu32_DataSetIndex, oru32_ArrayIndex, orc_NewData,
                                             this->mpc_ModelViewManager);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set model view manager

   \param[in,out] opc_Value Model view manager
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListArrayEditView::SetModelViewManager(C_SdNdeDpListModelViewManager * const opc_Value)
{
   this->mpc_ModelViewManager = opc_Value;
   m_UpdateModelView();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Get selected indices

   \return
   Selected indices
*/
//----------------------------------------------------------------------------------------------------------------------
std::vector<uint32_t> C_SdNdeDpListArrayEditView::m_GetSelectedIndices(void) const
{
   std::vector<uint32_t> c_Retval;
   QModelIndexList c_SelectedItems = this->selectedIndexes();

   c_Retval.reserve(c_SelectedItems.size());
   for (QModelIndexList::const_iterator c_ItSelectedItem = c_SelectedItems.begin();
        c_ItSelectedItem != c_SelectedItems.end(); ++c_ItSelectedItem)
   {
      const QModelIndex & rc_Item = *c_ItSelectedItem;
      c_Retval.push_back(rc_Item.column());
   }
   C_Uti::h_Uniqueify(c_Retval);
   return c_Retval;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update model
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListArrayEditView::m_UpdateModelView(void)
{
   if (this->mpc_ModelViewManager != NULL)
   {
      C_SdNdeDpListArrayEditModel * const pc_Model = this->mpc_ModelViewManager->GetArrayEditModel(
         this->mu32_NodeIndex, this->mu32_DataPoolIndex, this->mu32_ListIndex, this->mu32_ElementIndex,
         this->me_ArrayEditType, this->mu32_DataSetIndex);
      if (pc_Model != NULL)
      {
         this->setModel(pc_Model);
         this->mc_Delegate.SetModel(pc_Model);
      }
   }
   if (this->mpc_ModelViewManager != NULL)
   {
      this->mpc_ModelViewManager->RegisterArrayEditView(this->mu32_NodeIndex, this->mu32_DataPoolIndex,
                                                        this->mu32_ListIndex, this->mu32_ElementIndex,
                                                        this->me_ArrayEditType, this->mu32_DataSetIndex, this);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update corner button size
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeDpListArrayEditView::m_UpdateCornerButton(void)
{
   tgl_assert(this->mpc_LabelCorner != NULL);
   if (this->mpc_LabelCorner != NULL)
   {
      this->mpc_LabelCorner->setGeometry(0, 0, this->verticalHeader()->width(), this->horizontalHeader()->height());
   }
}
