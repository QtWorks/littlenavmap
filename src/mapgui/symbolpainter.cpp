#include "mapquery.h"
#include "symbolpainter.h"
#include "common/mapcolors.h"

#include <QPainter>

SymbolPainter::SymbolPainter()
{
}

void SymbolPainter::drawAirportSymbol(QPainter *painter, const MapAirport& ap, int x, int y, int size,
                                      bool isAirportDiagram, bool fast)
{
  painter->save();
  QColor apColor = mapcolors::colorForAirport(ap);

  int radius = size / 2;
  painter->setBackgroundMode(Qt::OpaqueMode);

  if(ap.isSet(HARD) && !ap.isSet(MIL) && !ap.isSet(CLOSED))
    // Use filled circle
    painter->setBrush(QBrush(apColor));
  else
    // Use white filled circle
    painter->setBrush(QBrush(mapcolors::airportSymbolFillColor));

  if(!fast || isAirportDiagram)
    if(ap.isSet(FUEL) && !ap.isSet(MIL) && !ap.isSet(CLOSED) && size > 6)
    {
      // Draw fuel spikes
      int fuelRadius = static_cast<int>(std::round(static_cast<double>(radius) * 1.4));
      if(fuelRadius < radius + 2)
        fuelRadius = radius + 2;
      painter->setPen(QPen(QBrush(apColor), size / 4, Qt::SolidLine, Qt::FlatCap));
      painter->drawLine(x, y - fuelRadius, x, y + fuelRadius);
      painter->drawLine(x - fuelRadius, y, x + fuelRadius, y);
    }

  painter->setPen(QPen(QBrush(apColor), size / 5, Qt::SolidLine, Qt::FlatCap));
  painter->drawEllipse(QPoint(x, y), radius, radius);

  if(!fast || isAirportDiagram)
  {
    if(ap.isSet(MIL))
      painter->drawEllipse(QPoint(x, y), radius / 2, radius / 2);

    if(ap.waterOnly() && size > 6)
    {
      int lineWidth = size / 7;
      painter->setPen(QPen(QBrush(apColor), lineWidth, Qt::SolidLine, Qt::FlatCap));
      painter->drawLine(x - lineWidth / 4, y - radius / 2, x - lineWidth / 4, y + radius / 2);
      painter->drawArc(x - radius / 2, y - radius / 2, radius, radius, 0 * 16, -180 * 16);
    }

    if(ap.isHeliport() && size > 6)
    {
      int lineWidth = size / 7;
      painter->setPen(QPen(QBrush(apColor), lineWidth, Qt::SolidLine, Qt::FlatCap));
      painter->drawLine(x - lineWidth / 4 - size / 5, y - radius / 2,
                        x - lineWidth / 4 - size / 5, y + radius / 2);

      painter->drawLine(x - lineWidth / 4 - size / 5, y,
                        x - lineWidth / 4 + size / 5, y);

      painter->drawLine(x - lineWidth / 4 + size / 5, y - radius / 2,
                        x - lineWidth / 4 + size / 5, y + radius / 2);
    }

    if(ap.isSet(CLOSED) && size > 6)
    {
      // Cross out whatever was painted before
      painter->setPen(QPen(QBrush(apColor), size / 7, Qt::SolidLine, Qt::FlatCap));
      painter->drawLine(x - radius, y - radius, x + radius, y + radius);
      painter->drawLine(x - radius, y + radius, x + radius, y - radius);
    }
  }

  if(!fast || isAirportDiagram)
    if(ap.isSet(HARD) && !ap.isSet(MIL) && !ap.isSet(CLOSED) && size > 6)
    {
      painter->translate(x, y);
      painter->rotate(ap.longestRunwayHeading);
      painter->setPen(QPen(QBrush(mapcolors::airportSymbolFillColor), size / 5, Qt::SolidLine, Qt::RoundCap));
      painter->drawLine(0, -radius + 2, 0, radius - 2);
      painter->resetTransform();
    }

  painter->restore();
}

void SymbolPainter::drawWaypointSymbol(QPainter *painter, const MapWaypoint& wp, int x, int y, int size,
                                       bool fast)
{
  Q_UNUSED(wp);
  painter->save();
  painter->setBrush(Qt::NoBrush);

  painter->setPen(QPen(mapcolors::waypointSymbolColor, 1.5, Qt::SolidLine, Qt::SquareCap));
  painter->setBackgroundMode(Qt::TransparentMode);

  if(!fast)
  {
    int radius = size / 2;
    QPolygon polygon;
    polygon << QPoint(x, y - radius)
            << QPoint(x + radius, y + radius)
            << QPoint(x - radius, y + radius);

    painter->drawConvexPolygon(polygon);
  }
  else
    painter->drawPoint(x, y);

  painter->restore();
}

void SymbolPainter::drawVorSymbol(QPainter *painter, const MapVor& vor, int x, int y, int size, bool fast)
{
  painter->save();
  painter->setBrush(Qt::NoBrush);
  painter->setPen(QPen(mapcolors::vorSymbolColor, 1.5, Qt::SolidLine, Qt::SquareCap));
  painter->setBackgroundMode(Qt::TransparentMode);

  if(!fast)
  {
    if(vor.hasDme)
      painter->drawRect(x - size / 2, y - size / 2, size, size);
    if(!vor.dmeOnly)
    {
      int radius = size / 2;
      int corner = 2;
      QPolygon polygon;
      polygon << QPoint(x - radius / corner, y - radius)
              << QPoint(x + radius / corner, y - radius)
              << QPoint(x + radius, y)
              << QPoint(x + radius / corner, y + radius)
              << QPoint(x - radius / corner, y + radius)
              << QPoint(x - radius, y);

      painter->drawConvexPolygon(polygon);
    }
  }

  if(size > 14)
    painter->setPen(QPen(mapcolors::vorSymbolColor, size / 4, Qt::SolidLine, Qt::RoundCap));
  else
    painter->setPen(QPen(mapcolors::vorSymbolColor, size / 3, Qt::SolidLine, Qt::RoundCap));
  painter->drawPoint(x, y);

  painter->drawPoint(x, y);
  painter->restore();
}

void SymbolPainter::drawNdbSymbol(QPainter *painter, const MapNdb& ndb, int x, int y, int size, bool fast)
{
  Q_UNUSED(ndb);
  painter->save();
  int radius = size / 2;

  painter->setBackgroundMode(Qt::TransparentMode);
  painter->setBrush(Qt::NoBrush);
  painter->setPen(QPen(mapcolors::ndbSymbolColor, 1.5, size > 12 ? Qt::DotLine : Qt::SolidLine, Qt::RoundCap));

  if(!fast)
    painter->drawEllipse(QPoint(x, y), radius, radius);
  if(size > 12)
  {
    if(!fast)
      painter->drawEllipse(QPoint(x, y), radius * 2 / 3, radius * 2 / 3);
    painter->setPen(QPen(mapcolors::ndbSymbolColor, size / 4, Qt::SolidLine, Qt::RoundCap));
  }
  else
    painter->setPen(QPen(mapcolors::ndbSymbolColor, size / 3, Qt::SolidLine, Qt::RoundCap));

  painter->drawPoint(x, y);

  painter->restore();
}

void SymbolPainter::drawMarkerSymbol(QPainter *painter, const MapMarker& marker, int x, int y, int size,
                                     bool fast)
{
  Q_UNUSED(marker);
  painter->save();
  int radius = size / 2;

  painter->setBackgroundMode(Qt::TransparentMode);
  painter->setBrush(Qt::NoBrush);
  painter->setPen(QPen(mapcolors::markerSymbolColor, 1.5, Qt::SolidLine, Qt::RoundCap));

  if(!fast)
  {
    painter->translate(x, y);
    painter->rotate(marker.heading);
    painter->drawEllipse(QPoint(0, 0), radius, radius / 2);
    painter->resetTransform();
  }

  painter->setPen(QPen(mapcolors::markerSymbolColor, size / 4, Qt::SolidLine, Qt::RoundCap));

  painter->drawPoint(x, y);

  painter->restore();
}