/*****************************************************************************
* Copyright 2015-2016 Alexander Barthel albar965@mailbox.org
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include "airporticondelegate.h"
#include "column.h"
#include "columnlist.h"
#include "sqlmodel.h"
#include "sqlproxymodel.h"
#include "logging/loggingdefs.h"
#include "mapgui/mapquery.h"
#include "mapgui/symbolpainter.h"
#include "common/mapcolors.h"

#include <QPainter>
#include <QSqlQueryModel>
#include <QApplication>

AirportIconDelegate::AirportIconDelegate(const ColumnList *columns)
  : cols(columns)
{
  symbolPainter = new SymbolPainter();
}

AirportIconDelegate::~AirportIconDelegate()
{
  delete symbolPainter;
}

void AirportIconDelegate::paint(QPainter *painter, const QStyleOptionViewItem& option,
                                const QModelIndex& index) const
{
  QModelIndex idx(index);
  const SqlModel *sqlModel = dynamic_cast<const SqlModel *>(index.model());
  if(sqlModel == nullptr)
  {
    const SqlProxyModel *sqlProxyModel = dynamic_cast<const SqlProxyModel *>(index.model());
    Q_ASSERT(sqlProxyModel != nullptr);
    sqlModel = dynamic_cast<const SqlModel *>(sqlProxyModel->sourceModel());
    idx = sqlProxyModel->mapToSource(index);
  }
  Q_ASSERT(sqlModel != nullptr);

  maptypes::MapAirport ap = mapAirport(sqlModel, idx.row());

  // Create a style copy
  QStyleOptionViewItem opt(option);
  opt.displayAlignment = Qt::AlignRight;
  if(ap.scenery())
    opt.font.setBold(true);
  if(ap.addon())
    opt.font.setItalic(true);

  // Draw the text
  QStyledItemDelegate::paint(painter, opt, index);

  painter->setRenderHint(QPainter::Antialiasing);
  int symSize = option.rect.height() - 4;
  symbolPainter->drawAirportSymbol(painter, ap,
                                   option.rect.x() + symSize,
                                   option.rect.y() + symSize / 2 + 2,
                                   symSize,
                                   false, false);
}

QVariant AirportIconDelegate::value(const SqlModel *sqlModel, int row, const QString& name) const
{
  const Column *col = cols->getColumn(name);
  Q_ASSERT(col != nullptr);

  QVariant data = sqlModel->getRawData(row, col->getIndex());

  if(data.isValid())
    return data;
  else
    qCritical() << "sibling for column" << name << "not found";

  return QVariant();
}

maptypes::MapAirport AirportIconDelegate::mapAirport(const SqlModel *sqlModel, int row) const
{
  using namespace maptypes;

  MapAirport ap;
  ap.longestRunwayHeading =
    static_cast<int>(std::roundf(value(sqlModel, row, "longest_runway_heading").toFloat()));

  // TODO duplicated from map query
  ap.flags |= flag(sqlModel, row, "num_helipad", AP_HELIPAD);
  ap.flags |= flag(sqlModel, row, "rating", AP_SCENERY);
  ap.flags |= flag(sqlModel, row, "has_avgas", AP_AVGAS);
  ap.flags |= flag(sqlModel, row, "has_jetfuel", AP_JETFUEL);
  ap.flags |= flag(sqlModel, row, "tower_frequency", AP_TOWER);
  ap.flags |= flag(sqlModel, row, "is_closed", AP_CLOSED);
  ap.flags |= flag(sqlModel, row, "is_military", AP_MIL);
  ap.flags |= flag(sqlModel, row, "is_addon", AP_ADDON);
  ap.flags |= flag(sqlModel, row, "num_runway_hard", AP_HARD);
  ap.flags |= flag(sqlModel, row, "num_runway_soft", AP_SOFT);
  ap.flags |= flag(sqlModel, row, "num_runway_water", AP_WATER);

  return ap;
}

maptypes::MapAirportFlags AirportIconDelegate::flag(const SqlModel *sqlModel, int row, const QString& field,
                                                    maptypes::MapAirportFlags flag) const
{
  QVariant val = value(sqlModel, row, field);
  if(val.isNull())
    return maptypes::AP_NONE;
  else
    return val.toInt() > 0 ? flag : maptypes::AP_NONE;
}
