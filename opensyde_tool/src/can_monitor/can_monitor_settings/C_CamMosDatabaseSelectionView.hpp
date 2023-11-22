//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       View for database message selection (header)

   See cpp file for detailed description

   \copyright   Copyright 2019 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMOSDATABASESELECTIONVIEW_HPP
#define C_CAMMOSDATABASESELECTIONVIEW_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <array>

#include "C_OgeTreeViewToolTipBase.hpp"
#include "C_TblTreDataElementSortFilter.hpp"
#include "C_CamMosDatabaseSelectionModel.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMosDatabaseSelectionView :
   public stw::opensyde_gui_elements::C_OgeTreeViewToolTipBase
{
   Q_OBJECT

public:
   C_CamMosDatabaseSelectionView(QWidget * const opc_Parent = NULL);
   ~C_CamMosDatabaseSelectionView(void) override;

   void Init(void);
   void Search(const QString & orc_Text);

   std::vector<std::array<QString, 2> > GetSelectedDataElements(void) const;
   bool IsEmpty(void) const;
   void SaveExpandedIndices(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigAccept(void);
   void SigSelectionChanged(const int32_t os32_Count);

protected:
   void mouseDoubleClickEvent(QMouseEvent * const opc_Event) override;
   void selectionChanged(const QItemSelection & orc_Selected, const QItemSelection & orc_Deselected) override;

private:
   stw::opensyde_gui_logic::C_CamMosDatabaseSelectionModel mc_Model;
   stw::opensyde_gui_logic::C_TblTreDataElementSortFilter mc_SortModel;
   static std::vector<QString> mhc_LastKnownExpandedTopLevelItems;

   void m_AppendExpandedIndices(std::vector<QString> & orc_FoundItems);
   void m_RestoreExpandedIndices(void);
   QModelIndex m_ManualMapFromSource(const QModelIndex & orc_Index) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
