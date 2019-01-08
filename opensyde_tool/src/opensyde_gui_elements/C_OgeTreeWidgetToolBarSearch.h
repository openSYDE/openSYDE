//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Tree widget for toolbar search (header)

   Tree widget for toolbar search (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     15.03.2017  STW/S.Singer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_OGETREEWIDGETTOOLBARSEARCH_H
#define C_OGETREEWIDGETTOOLBARSEARCH_H

/* -- Includes ------------------------------------------------------------- */

#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace stw_opensyde_gui_elements
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_OgeTreeWidgetToolBarSearch :
   public QTreeWidget
{
   Q_OBJECT

public:
   C_OgeTreeWidgetToolBarSearch(QWidget * const opc_Parent = NULL);
   virtual ~C_OgeTreeWidgetToolBarSearch();

   virtual bool Search(const QString & orc_SearchString) = 0;
   virtual void ClearResult(void) = 0;
   virtual void SetSearchResultFocus(void) = 0;
   bool HasResults(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChangeMode(const stw_types::sint32 os32_Mode, const stw_types::sint32 os32_SubMode,
                      const stw_types::uint32 ou32_Index, const QString & orc_Name, const QString & orc_SubSubItemName,
                      const stw_types::uint32 ou32_Flag = 0U);
   void SigOpenDetail(const stw_types::sint32 os32_Index, const stw_types::sint32 os32_SubIndex,
                      const stw_types::sint32 os32_SubSubIndex, const stw_types::sint32 os32_SubSubSubIndex,
                      const stw_types::sint32 os32_Flag);

protected:
   void m_DeleteChildren(QTreeWidgetItem * const opc_ItemParent) const;
   void m_MarkResultString(QString & orc_ResultString) const;

   QString mc_SearchString;
   bool mq_ResultsFound;
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
