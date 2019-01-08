//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Data pool list array edit view (implementation)

   Data pool list array edit view

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     06.11.2017  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QScrollBar>
#include "C_Uti.h"
#include "stwtypes.h"
#include "C_GtGetText.h"
#include "C_SyvDaItPaArView.h"
#include "C_SdNdeSingleHeaderView.h"
#include "TGLUtils.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_gui_logic;

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

   \param[in,out] opc_Parent Optional pointer to parent

   \created     06.11.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaItPaArView::C_SyvDaItPaArView(QWidget * const opc_Parent) :
   C_TblViewScroll(opc_Parent),
   mc_Model(),
   mc_Delegate(),
   mpc_LabelCorner(NULL)
{
   //UI Settings
   this->setCornerButtonEnabled(false);
   this->setSortingEnabled(false);
   this->setGridStyle(Qt::NoPen);
   this->setShowGrid(false);
   this->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectColumns);
   this->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
   this->setAlternatingRowColors(true);
   this->setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
   this->setDefaultDropAction(Qt::DropAction::MoveAction);
   this->setDragEnabled(true);
   this->setLineWidth(0);
   this->setFrameShadow(QAbstractItemView::Shadow::Plain);
   this->setFrameShape(QAbstractItemView::Shape::NoFrame);
   this->setEditTriggers(
      QAbstractItemView::DoubleClicked | QAbstractItemView::CurrentChanged | QAbstractItemView::SelectedClicked);
   //Consider all elements for resize
   this->setVerticalHeader(new C_SdNdeSingleHeaderView(Qt::Vertical));
   this->verticalHeader()->setResizeContentsPrecision(-1);
   this->verticalHeader()->setDefaultSectionSize(40);
   //Row Height
   this->setHorizontalHeader(new C_SdNdeSingleHeaderView(Qt::Horizontal));
   this->horizontalHeader()->setResizeContentsPrecision(-1);
   this->horizontalHeader()->setDefaultSectionSize(70);
   this->horizontalHeader()->setFixedHeight(35);
   //Corner button
   this->setCornerButtonEnabled(false);

   this->setModel(&mc_Model);
   this->mc_Delegate.SetModel(&mc_Model);
   this->setItemDelegate(&mc_Delegate);

   //Hover event
   this->setMouseTracking(true);

   //Corner button label
   this->mpc_LabelCorner = new QLabel(this);
   if (this->mpc_LabelCorner != NULL)
   {
      this->mpc_LabelCorner->setAlignment(Qt::AlignCenter);
      this->mpc_LabelCorner->setAttribute(Qt::WA_TransparentForMouseEvents);
      this->mpc_LabelCorner->setText(C_GtGetText::h_GetText("Index"));
      connect(
         this->verticalHeader(), &QHeaderView::geometriesChanged, this,
         &C_SyvDaItPaArView::m_UpdateCornerButton);
      connect(
         this->horizontalHeader(), &QHeaderView::geometriesChanged, this,
         &C_SyvDaItPaArView::m_UpdateCornerButton);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Clean up.

   \created     17.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
C_SyvDaItPaArView::~C_SyvDaItPaArView(void)
{
   mpc_LabelCorner = NULL;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Specify associated list

   \param[in] oru32_ElementIndex Element index
   \param[in] opc_DataWidget     Data widget
   \param[in] oq_ECUValues       Optional flag if the shown values are ECU values

   \created     07.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItPaArView::SetElement(const uint32 ou32_ElementIndex,
                                   stw_opensyde_gui_logic::C_PuiSvDbDataElementHandler * const opc_DataWidget,
                                   const bool oq_ECUValues)
{
   this->mc_Model.SetElement(ou32_ElementIndex, opc_DataWidget, oq_ECUValues);
}

//-----------------------------------------------------------------------------
/*!
   \brief   Forward signal

   \created     27.02.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItPaArView::OnErrorChangePossible(void)
{
   Q_EMIT this->SigErrorChangePossible();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Get selected indices

   \return
   Selected indices

   \created     27.01.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
std::vector<uint32> C_SyvDaItPaArView::m_GetSelectedIndices(void) const
{
   const QModelIndexList c_SelectedItems = this->selectedIndexes();

   std::vector<uint32> c_Retval;

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

//-----------------------------------------------------------------------------
/*!
   \brief   Update corner button size

   \created     20.03.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SyvDaItPaArView::m_UpdateCornerButton(void)
{
   tgl_assert(this->mpc_LabelCorner != NULL);
   if (this->mpc_LabelCorner != NULL)
   {
      this->mpc_LabelCorner->setGeometry(0, 0, this->verticalHeader()->width(), this->horizontalHeader()->height());
   }
}
