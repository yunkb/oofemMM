/*
 *
 *                 #####    #####   ######  ######  ###   ###
 *               ##   ##  ##   ##  ##      ##      ## ### ##
 *              ##   ##  ##   ##  ####    ####    ##  #  ##
 *             ##   ##  ##   ##  ##      ##      ##     ##
 *            ##   ##  ##   ##  ##      ##      ##     ##
 *            #####    #####   ##      ######  ##     ##
 *
 *
 *             OOFEM : Object Oriented Finite Element Code
 *
 *               Copyright (C) 1993 - 2013   Borek Patzak
 *
 *
 *
 *       Czech Technical University, Faculty of Civil Engineering,
 *   Department of Structural Mechanics, 166 29 Prague, Czech Republic
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "Elements/PlaneStrain/qplanestrain.h"
#include "fei2dquadquad.h"
#include "classfactory.h"
#include "gausspoint.h"


#ifdef __OOFEG
 #include "oofeggraphiccontext.h"
#endif

namespace oofem {
REGISTER_Element(QPlaneStrain);

FEI2dQuadQuad QPlaneStrain :: interpolation(1, 2);

QPlaneStrain :: QPlaneStrain(int n, Domain *aDomain) :
    PlaneStrainElement(n, aDomain), ZZNodalRecoveryModelInterface(this), SpatialLocalizerInterface(this)
{
    numberOfDofMans  = 8;
    numberOfGaussPoints = 4;
}

FEInterpolation *
QPlaneStrain :: giveInterpolation() const { return & interpolation; }

Interface *
QPlaneStrain :: giveInterface(InterfaceType interface)
{
    if ( interface == ZZNodalRecoveryModelInterfaceType ) {
        return static_cast< ZZNodalRecoveryModelInterface * >(this);
    } else if ( interface == NodalAveragingRecoveryModelInterfaceType ) {
        return static_cast< NodalAveragingRecoveryModelInterface * >(this);
    } else if ( interface == SpatialLocalizerInterfaceType ) {
        return static_cast< SpatialLocalizerInterface * >(this);
    }

    return NULL;
}

void
QPlaneStrain :: NodalAveragingRecoveryMI_computeNodalValue(FloatArray &answer, int node,  InternalStateType type, TimeStep *tStep)
{

  int size = 0;
  double scale = 0.;
  FloatArray val;
  int iGp;
  for ( GaussPoint *gp: *integrationRulesArray [ 0 ] ) {
    
    iGp = gp->giveNumber();
    giveIPValue(val, gp, type, tStep);
    
    /*   FloatArray stresses  =  {2.3211e+03, 2.3262e+03, 1.9537e+03, 1.9584e+03};
    val.at(1) = stresses.at(iGp);
    */
    if(size == 0) {
      size = val.giveSize();
      answer.resize(size);
      answer.zero();
    }

    if(scale == 0.) {
      scale = fabs(gp->giveNaturalCoordinates().at(1));
    }

    
    FloatArray coords;
    // gp->giveNaturalCoordinates();

    switch ( node ) {
      /*case 1:
      coords = {-1.0,-1.0};
      break;
    case 2:
      coords = {1.0,-1.0};
      break;
    case 3:
      coords = {1.0,1.0};
      break;
    case 4:
      coords = {-1.0,1.0};
      break;
    case 5:
      coords = {0.0,-1.0};
      break;
    case 6:
      coords = {1.0,0.0};
      break;
    case 7:
      coords = {0.0,1.0};
      break;
    case 8:
      coords = {-1.0,0.0};
      break;
      */
    case 1:
      coords = {1.0,-1.0};
      break;
    case 2:
      coords = {1.0,1.0};
      break;
    case 3:
      coords = {-1.0,1.0};
      break;
    case 4:
      coords = {-1.0,-1.0};
      break;
    case 5:
      coords = {1.0,0.0};
      break;
    case 6:
      coords = {0.0,1.0};
      break;
    case 7:
      coords = {-1.0,0.0};
      break;
    case 8:
      coords = {0.0,-1.0};
      break;
      
    default:
      OOFEM_ERROR("unsupported node");
    }
    


    if(scale != 0) {
      coords.times(1./scale);
    } else {
      coords.zero();
    }
    FloatArray n, nodalValue;
    //this->giveInterpolation()->evalN(n, coords, *this->giveCellGeometryWrapper());
    double ksi = coords.at(1);
    double eta = coords.at(2);
    n = {
      ( 1. - ksi ) * ( 1. - eta ) * 0.25,
      ( 1. + ksi ) * ( 1. - eta ) * 0.25,
      ( 1. + ksi ) * ( 1. + eta ) * 0.25,
      ( 1. - ksi ) * ( 1. + eta ) * 0.25        
    };    




    for ( int j = 1; j <= size; j++ ) {
      answer.at(j) += n.at(iGp)*val.at(j);
    }
    
  }
  

}






#ifdef __OOFEG
void QPlaneStrain :: drawRawGeometry(oofegGraphicContext &gc, TimeStep *tStep)
{
    WCRec p [ 4 ];
    GraphicObj *go;

    if ( !gc.testElementGraphicActivity(this) ) {
        return;
    }

    EASValsSetLineWidth(OOFEG_RAW_GEOMETRY_WIDTH);
    EASValsSetColor( gc.getElementColor() );
    EASValsSetEdgeColor( gc.getElementEdgeColor() );
    EASValsSetEdgeFlag(true);
    EASValsSetLayer(OOFEG_RAW_GEOMETRY_LAYER);
    EASValsSetFillStyle(FILL_HOLLOW);
    p [ 0 ].x = ( FPNum ) this->giveNode(1)->giveCoordinate(1);
    p [ 0 ].y = ( FPNum ) this->giveNode(1)->giveCoordinate(2);
    p [ 0 ].z = 0.;
    p [ 1 ].x = ( FPNum ) this->giveNode(2)->giveCoordinate(1);
    p [ 1 ].y = ( FPNum ) this->giveNode(2)->giveCoordinate(2);
    p [ 1 ].z = 0.;
    p [ 2 ].x = ( FPNum ) this->giveNode(3)->giveCoordinate(1);
    p [ 2 ].y = ( FPNum ) this->giveNode(3)->giveCoordinate(2);
    p [ 2 ].z = 0.;
    p [ 3 ].x = ( FPNum ) this->giveNode(4)->giveCoordinate(1);
    p [ 3 ].y = ( FPNum ) this->giveNode(4)->giveCoordinate(2);
    p [ 3 ].z = 0.;

    go =  CreateQuad3D(p);
    EGWithMaskChangeAttributes(WIDTH_MASK | FILL_MASK | COLOR_MASK | EDGE_COLOR_MASK | EDGE_FLAG_MASK | LAYER_MASK, go);
    EGAttachObject(go, ( EObjectP ) this);
    EMAddGraphicsToModel(ESIModel(), go);
}


void QPlaneStrain :: drawDeformedGeometry(oofegGraphicContext &gc, TimeStep *tStep, UnknownType type)
{
    WCRec p [ 4 ];
    GraphicObj *go;
    double defScale = gc.getDefScale();

    if ( !gc.testElementGraphicActivity(this) ) {
        return;
    }

    EASValsSetLineWidth(OOFEG_DEFORMED_GEOMETRY_WIDTH);
    EASValsSetColor( gc.getDeformedElementColor() );
    EASValsSetEdgeColor( gc.getElementEdgeColor() );
    EASValsSetEdgeFlag(true);
    EASValsSetLayer(OOFEG_DEFORMED_GEOMETRY_LAYER);
    EASValsSetFillStyle(FILL_HOLLOW);
    p [ 0 ].x = ( FPNum ) this->giveNode(1)->giveUpdatedCoordinate(1, tStep, defScale);
    p [ 0 ].y = ( FPNum ) this->giveNode(1)->giveUpdatedCoordinate(2, tStep, defScale);
    p [ 0 ].z = 0.;
    p [ 1 ].x = ( FPNum ) this->giveNode(2)->giveUpdatedCoordinate(1, tStep, defScale);
    p [ 1 ].y = ( FPNum ) this->giveNode(2)->giveUpdatedCoordinate(2, tStep, defScale);
    p [ 1 ].z = 0.;
    p [ 2 ].x = ( FPNum ) this->giveNode(3)->giveUpdatedCoordinate(1, tStep, defScale);
    p [ 2 ].y = ( FPNum ) this->giveNode(3)->giveUpdatedCoordinate(2, tStep, defScale);
    p [ 2 ].z = 0.;
    p [ 3 ].x = ( FPNum ) this->giveNode(4)->giveUpdatedCoordinate(1, tStep, defScale);
    p [ 3 ].y = ( FPNum ) this->giveNode(4)->giveUpdatedCoordinate(2, tStep, defScale);
    p [ 3 ].z = 0.;

    go =  CreateQuad3D(p);
    EGWithMaskChangeAttributes(WIDTH_MASK | FILL_MASK | COLOR_MASK | EDGE_COLOR_MASK | EDGE_FLAG_MASK | LAYER_MASK, go);
    EMAddGraphicsToModel(ESIModel(), go);
}


void QPlaneStrain :: drawScalar(oofegGraphicContext &gc, TimeStep *tStep)
{
    int i, indx;
    WCRec p [ 4 ];
    GraphicObj *tr;
    FloatArray v [ 4 ];
    double s [ 4 ], defScale;

    if ( !gc.testElementGraphicActivity(this) ) {
        return;
    }

    EASValsSetLayer(OOFEG_VARPLOT_PATTERN_LAYER);
    if ( gc.giveIntVarMode() == ISM_recovered ) {
        // ============ plot the recovered values (smoothed data) ===============
        /*
         * for ( i = 1; i <= 4; i++ ) {
         *    result += this->giveInternalStateAtNode(v [ i - 1 ], gc.giveIntVarType(), gc.giveIntVarMode(), i, tStep);
         * }
         *
         * if ( result != 4 ) {
         *    return;
         * }
         *
         * indx = gc.giveIntVarIndx();
         *
         * for ( i = 1; i <= 4; i++ ) {
         *    s [ i - 1 ] = v [ i - 1 ].at(indx);
         * }
         *
         * if ( gc.getScalarAlgo() == SA_ISO_SURF ) {
         *    for ( i = 0; i < 4; i++ ) {
         *        if ( gc.getInternalVarsDefGeoFlag() ) {
         *            // use deformed geometry
         *            defScale = gc.getDefScale();
         *            p [ i ].x = ( FPNum ) this->giveNode(i + 1)->giveUpdatedCoordinate(1, tStep, defScale);
         *            p [ i ].y = ( FPNum ) this->giveNode(i + 1)->giveUpdatedCoordinate(2, tStep, defScale);
         *            p [ i ].z = 0.;
         *        } else {
         *            p [ i ].x = ( FPNum ) this->giveNode(i + 1)->giveCoordinate(1);
         *            p [ i ].y = ( FPNum ) this->giveNode(i + 1)->giveCoordinate(2);
         *            p [ i ].z = 0.;
         *        }
         *    }
         *
         *    //EASValsSetColor(gc.getYieldPlotColor(ratio));
         *    gc.updateFringeTableMinMax(s, 4);
         *    tr =  CreateQuadWD3D(p, s [ 0 ], s [ 1 ], s [ 2 ], s [ 3 ]);
         *    EGWithMaskChangeAttributes(LAYER_MASK, tr);
         *    EMAddGraphicsToModel(ESIModel(), tr);
         * } else if ( ( gc.getScalarAlgo() == SA_ZPROFILE ) || ( gc.getScalarAlgo() == SA_COLORZPROFILE ) ) {
         *    double landScale = gc.getLandScale();
         *
         *    for ( i = 0; i < 4; i++ ) {
         *        if ( gc.getInternalVarsDefGeoFlag() ) {
         *            // use deformed geometry
         *            defScale = gc.getDefScale();
         *            p [ i ].x = ( FPNum ) this->giveNode(i + 1)->giveUpdatedCoordinate(1, tStep, defScale);
         *            p [ i ].y = ( FPNum ) this->giveNode(i + 1)->giveUpdatedCoordinate(2, tStep, defScale);
         *            p [ i ].z = s [ i ] * landScale;
         *        } else {
         *            p [ i ].x = ( FPNum ) this->giveNode(i + 1)->giveCoordinate(1);
         *            p [ i ].y = ( FPNum ) this->giveNode(i + 1)->giveCoordinate(2);
         *            p [ i ].z = s [ i ] * landScale;
         *        }
         *
         *        // this fixes a bug in ELIXIR
         *        if ( fabs(s [ i ]) < 1.0e-6 ) {
         *            s [ i ] = 1.0e-6;
         *        }
         *    }
         *
         *    if ( gc.getScalarAlgo() == SA_ZPROFILE ) {
         *        EASValsSetColor( gc.getDeformedElementColor() );
         *        EASValsSetLineWidth(OOFEG_DEFORMED_GEOMETRY_WIDTH);
         *        tr =  CreateQuad3D(p);
         *        EGWithMaskChangeAttributes(WIDTH_MASK | COLOR_MASK | LAYER_MASK, tr);
         *    } else {
         *        gc.updateFringeTableMinMax(s, 4);
         *        tr =  CreateQuadWD3D(p, s [ 0 ], s [ 1 ], s [ 2 ], s [ 3 ]);
         *        EGWithMaskChangeAttributes(LAYER_MASK, tr);
         *    }
         *
         *    EMAddGraphicsToModel(ESIModel(), tr);
         * }
         */
    } else if ( gc.giveIntVarMode() == ISM_local ) {
        // ========== plot the local values (raw data) =====================
        if ( numberOfGaussPoints != 4 ) {
            return;
        }

        IntArray ind(4);
        FloatArray *gpCoords;
        WCRec pp [ 9 ];

        for ( i = 0; i < 8; i++ ) {
            if ( gc.getInternalVarsDefGeoFlag() ) {
                // use deformed geometry
                defScale = gc.getDefScale();
                pp [ i ].x = ( FPNum ) this->giveNode(i + 1)->giveUpdatedCoordinate(1, tStep, defScale);
                pp [ i ].y = ( FPNum ) this->giveNode(i + 1)->giveUpdatedCoordinate(2, tStep, defScale);
                pp [ i ].z = 0.;
            } else {
                pp [ i ].x = ( FPNum ) this->giveNode(i + 1)->giveCoordinate(1);
                pp [ i ].y = ( FPNum ) this->giveNode(i + 1)->giveCoordinate(2);
                pp [ i ].z = 0.;
            }
        }

        pp [ 8 ].x = 0.25 * ( pp [ 0 ].x + pp [ 1 ].x + pp [ 2 ].x + pp [ 3 ].x );
        pp [ 8 ].y = 0.25 * ( pp [ 0 ].y + pp [ 1 ].y + pp [ 2 ].y + pp [ 3 ].y );
        pp [ 8 ].z = 0.;

        for ( auto &gp: *this->giveDefaultIntegrationRulePtr() ) {
            gpCoords = gp->giveNaturalCoordinates();
            if ( ( gpCoords->at(1) > 0. ) && ( gpCoords->at(2) > 0. ) ) {
                ind.at(1) = 0;
                ind.at(2) = 4;
                ind.at(3) = 8;
                ind.at(4) = 7;
            } else if ( ( gpCoords->at(1) < 0. ) && ( gpCoords->at(2) > 0. ) ) {
                ind.at(1) = 4;
                ind.at(2) = 1;
                ind.at(3) = 5;
                ind.at(4) = 8;
            } else if ( ( gpCoords->at(1) < 0. ) && ( gpCoords->at(2) < 0. ) ) {
                ind.at(1) = 5;
                ind.at(2) = 2;
                ind.at(3) = 6;
                ind.at(4) = 8;
            } else {
                ind.at(1) = 6;
                ind.at(2) = 3;
                ind.at(3) = 7;
                ind.at(4) = 8;
            }

            if ( giveIPValue(v [ 0 ], gp, gc.giveIntVarType(), tStep) == 0 ) {
                return;
            }

            indx = gc.giveIntVarIndx();

            for ( i = 1; i <= 4; i++ ) {
                s [ i - 1 ] = v [ 0 ].at(indx);
            }

            for ( i = 0; i < 4; i++ ) {
                p [ i ].x = pp [ ind.at(i + 1) ].x;
                p [ i ].y = pp [ ind.at(i + 1) ].y;
                p [ i ].z = pp [ ind.at(i + 1) ].z;
            }

            gc.updateFringeTableMinMax(s, 4);
            tr =  CreateQuadWD3D(p, s [ 0 ], s [ 1 ], s [ 2 ], s [ 3 ]);
            EGWithMaskChangeAttributes(LAYER_MASK, tr);
            EMAddGraphicsToModel(ESIModel(), tr);
        }
    }
}

#endif
} // end namespace oofem
