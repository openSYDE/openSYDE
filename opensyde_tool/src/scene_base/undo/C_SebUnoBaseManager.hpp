//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Handler class for scene undo command stack (header)

   See cpp file for detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SEBUNOBASEMANAGER_HPP
#define C_SEBUNOBASEMANAGER_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QList>

#include "stwtypes.hpp"

#include "C_UtiUndoStack.hpp"

#include "C_SebUnoMoveCommandGroup.hpp"
#include "C_SebUnoMoveCommand.hpp"
#include "C_SebUnoSetupStyleCommand.hpp"
#include "C_SebUtil.hpp"
#include "C_GiLiLineGroup.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui_logic
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SebUnoBaseManager :
   public C_UtiUndoStack
{
   Q_OBJECT

public:
   C_SebUnoBaseManager(QGraphicsScene * const opc_Scene, QObject * const opc_Parent = NULL);
   ~C_SebUnoBaseManager(void) override;

   void RegisterMove(void);
   void DoMove(const QList<QGraphicsItem *> & orc_Items, const QPointF & orc_PositionDifference);
   virtual void RegisterResizeLine(const QList<QGraphicsItem *> & orc_Items, const int32_t & ors32_InteractionPointId,
                                   const QPointF & orc_PositionDifference);
   void RegisterResizeRectangle(const QList<QGraphicsItem *> & orc_Items, const QPointF & orc_OldPos,
                                const QSizeF & orc_OldSize, const QPointF & orc_NewPos, const QSizeF & orc_NewSize);
   virtual void RegisterCompleteMoveStep(const QList<QGraphicsItem *> & orc_Items,
                                         const QPointF & orc_PositionDifference);
   virtual void RegisterMoveSubLine(const QList<QGraphicsItem *> & orc_Items, const QPointF & orc_PositionDifference,
                                    const int32_t & ors32_SubLineId);
   void DoBendLine(const stw::opensyde_gui::C_GiLiLineGroup * const opc_Line, const QPointF & orc_ScenePos);
   void DoRemoveBendLine(const stw::opensyde_gui::C_GiLiLineGroup * const opc_Line, const QPointF & orc_ScenePos);

   virtual void DoDelete(const QList<QGraphicsItem *> & orc_Items) = 0;

   void DoAlign(const QList<QGraphicsItem *> & orc_Items, const QGraphicsItem * const opc_AlignItem,
                const E_Alignment & ore_Alignment);
   virtual void AdaptZetOrder(const QList<QGraphicsItem *> & orc_SelectedItems,
                              const QList<QGraphicsItem *> & orc_Items, const bool oq_BringToFront);
   void SaveStyleInformation(const QList<QGraphicsItem *> & orc_Items, const bool oq_DarkMode);
   void RegisterStyleChange(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChanged(void);
   void SigErrorChanged(void);

protected:
   static void mh_MapItemToId(const QList<QGraphicsItem *> & orc_Items, std::vector<uint64_t> & orc_Ids);
   static void mh_MapItemToId(const QGraphicsItem * const opc_Item, uint64_t & oru64_Id);
   virtual C_SebUnoSetupStyleCommand * m_GetNewStyleCommand(const std::vector<uint64_t> & orc_Items,
                                                            const bool oq_DarkMode);

   C_SebUnoSetupStyleCommand * mpc_StyleCommand;
   QGraphicsScene * const mpc_Scene;

private:
   C_SebUnoMoveCommandGroup * mpc_MoveCommandGroup;
   C_SebUnoMoveCommand * mpc_LastMoveCommand;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
