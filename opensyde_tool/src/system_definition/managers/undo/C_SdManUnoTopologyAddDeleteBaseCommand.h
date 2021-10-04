//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Add and delete base class for system definition items (header)

   See cpp file for detailed description

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDMANUNOTOPOLOGYADDDELETEBASECOMMAND_H
#define C_SDMANUNOTOPOLOGYADDDELETEBASECOMMAND_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */

#include <QMap>
#include <QObject>
#include "C_GiNode.h"
#include "C_SebUnoAddDeleteBaseCommand.h"
#include "C_PuiSdDataElement.h"
#include "C_SdTopologyDataSnapshot.h"
#include "C_PuiBsTemporaryDataID.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw_opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdManUnoTopologyAddDeleteBaseCommand :
   public QObject,
   public C_SebUnoAddDeleteBaseCommand

{
   Q_OBJECT

public:
   C_SdManUnoTopologyAddDeleteBaseCommand(QGraphicsScene * const opc_Scene,
                                          const std::vector<stw_types::uint64> & orc_IDs, const QString & orc_Text,
                                          QUndoCommand * const opc_Parent = NULL, const C_SdTopologyDataSnapshot & orc_InitialSnapshotData =
                                             C_SdTopologyDataSnapshot());
   virtual ~C_SdManUnoTopologyAddDeleteBaseCommand(void);
   static void h_GetAllRelevantObjects(const QList<QGraphicsItem *> & orc_AllItems,
                                       const QList<QGraphicsItem *> & orc_SelectedItems,
                                       QList<QGraphicsItem *> & orc_AllRelevantItems);
   C_SdTopologyDataSnapshot GetDataBackup() const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigErrorChange(void);

protected:
   virtual void m_DeleteSave(void);
   virtual void m_Restore(void);

   virtual stw_types::sint32 m_GetBoundaryType(void) const;
   virtual stw_types::sint32 m_GetImageType(void) const;
   virtual stw_types::sint32 m_GetLineArrowType(void) const;
   virtual stw_types::sint32 m_GetTextElementType(void) const;

   void m_SetDataPositionOffset(const QPointF & orc_NewPos);
   void m_SetDataZOffset(const stw_types::float64 of64_HighestUsedZValue);

private:
   QMap<C_PuiBsTemporaryDataID, stw_types::uint64> mc_MapTypeAndIndexToID;
   C_SdTopologyDataSnapshot mc_DataBackup;

   static void mh_SearchAndAddAllAffectedBusConnectorsToTheFront(const QList<QGraphicsItem *> & orc_AllItems,
                                                                 const stw_types::sint32 & ors32_BusIndex,
                                                                 const stw_types::sint32 & ors32_NodeIndex,
                                                                 QList<QGraphicsItem *> & orc_AllRelevantItems);
   static void mh_SearchAndAddAllAffectedBusTextElementsToTheFront(const QList<QGraphicsItem *> & orc_AllItems,
                                                                   const stw_types::uint32 ou32_BusIndex,
                                                                   QList<QGraphicsItem *> & orc_AllRelevantItems);
   void m_SaveToData(void);
   void m_SaveToDataNode(const stw_opensyde_gui::C_GiNode * const opc_UiNodeItem,
                         const stw_types::uint32 ou32_NodeIndex, const stw_types::uint64 ou64_UniqueID,
                         stw_types::uint32 & oru32_NodeCounter);
   void m_SaveToDataMultiNode(const stw_types::uint32 ou32_NodeIndex);
   void m_Clear(void);
   void m_Delete(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
