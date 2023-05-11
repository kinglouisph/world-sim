const int mapWidth = 150;
const int mapHeight = 150;
const int mapSize = mapWidth * mapHeight;


typedef struct World {
  float* elevation;
  float* rainfall;
  vec3* colorGrid;
  
  
  
  

} World;


//returns row major grids for elevation, moisture.
World generateWorldGrids() {
  World world;
  
  //elevation
  
	
	int bigLines = 8;
	int smallLines = 15;
	int totalLines = bigLines + smallLines;
	
	float* mapGrid = malloc(4*mapSize);
	float faultLines[totalLines][5]; //p1, p2, "width"
	float lineDrawPoints[totalLines][2][2];
	
	
	
	
	//generate lines to form mountain rangus and peninsulae better
	for (int i = 0; i < bigLines; i++) {
		float x1 = randf() * mapWidth;
		float y1 = randf() * mapHeight;
		float x2 = randf() * mapWidth;
		float y2 = randf() * mapHeight;
		
		if (x2 < x1) {
			float t = x1;
			x1 = x2;
			x2 = t;
			t = y1;
			y1 = y2;
			y2 = t;
		} 
		
		
		float m = (y1-y2)/(x1-x2);
		float b = y1-m*x1;
		
		char cont = 0;
		for (int j = 0;j<i;j++) {
			float x3 = faultLines[j][0];
			float y3 = faultLines[j][1];
			float x4 = faultLines[j][2];
			float y4 = faultLines[j][3];
			
			float m2 = (y3-y4)/(x3-x4);
			float b2 = y3 - m2*x3;
			float t = (b2-b) / (m-m2);
			if (t > x1 && t < x2 && t > x3 && t < x4) {
				i--;
				cont=1;
				break;
			}
		}
		if (cont) {continue;}
		
		faultLines[i][0] = x1;
		faultLines[i][1] = y1;
		faultLines[i][2] = x2;
		faultLines[i][3] = y2;
		
		lineDrawPoints[i][0][0] = faultLines[i][0];
		lineDrawPoints[i][0][1] = faultLines[i][1];
		lineDrawPoints[i][1][0] = faultLines[i][2];
		lineDrawPoints[i][1][1] = faultLines[i][3];
	}
	
	for (int i = 0; i < smallLines; i++) {
		float x1 = randf() * mapWidth;
		float y1 = randf() * mapHeight;
		float a = randf() * M_PI * 2;
		float l = randf() * 15 + 10;
		float x2 = cos(a)*l+x1;
		float y2 = sin(a)*l+y1;
				
		if (x2 < x1) {
			float t = x1;
			x1 = x2;
			x2 = t;
			t = y1;
			y1 = y2;
			y2 = t;
		} 
		
		i+=bigLines;
		faultLines[i][0] = x1;
		faultLines[i][1] = y1;
		faultLines[i][2] = x2;
		faultLines[i][3] = y2;
		
		lineDrawPoints[i][0][0] = faultLines[i][0];
		lineDrawPoints[i][0][1] = faultLines[i][1];
		lineDrawPoints[i][1][0] = faultLines[i][2];
		lineDrawPoints[i][1][1] = faultLines[i][3];
		i-=bigLines;
	}
	
	//setup world grid
	for(int i=0;i<mapWidth;i++) {
		for(int j=0;j<mapHeight;j++) {
			int pos = i+j*mapWidth;
			mapGrid[pos]=randf()*20.0f;
			
			for (int k = 0; k < totalLines;k++) {
				float d = pointLineDist(i+0.5f,j+0.5f,faultLines[k][0],faultLines[k][1],faultLines[k][2],faultLines[k][3]);
				mapGrid[pos]+=sqrt(1.0f*mapSize)*1.0f/(d+4.0f);
			}
			
			
		}
	}
  
  float* t = mapGrid;
	mapGrid = diffuse2d(mapGrid, mapWidth, mapHeight,0.05f);
	free(t);
	t = mapGrid;
	mapGrid = diffuse2d(mapGrid, mapWidth, mapHeight,0.08f);
	free(t);
	t = mapGrid;
	mapGrid = diffuse2d(mapGrid, mapWidth, mapHeight,0.1f);
	free(t);
	t = mapGrid;
	mapGrid = diffuse2d(mapGrid, mapWidth, mapHeight,0.1f);
	free(t);
  t = mapGrid;
	mapGrid = diffuse2d(mapGrid, mapWidth, mapHeight,0.1f);
	free(t);
  world.elevation = mapGrid;
  
  
  
  
  
  
  //generate color grids. Do last
  //world.redGrid = malloc(4*mapSize);
  //world.greenGrid = malloc(4*mapSize);
  //world.blueGrid = malloc(4*mapSize);
  world.colorGrid = malloc(12*mapSize);
  
  float oceanThreshold = 90.0f;
  float mountainThreshold = 115.0f;
  
  for (int i = 0; i < mapWidth; i++) {
    for (int j = 0; j < mapHeight; j++) {
      int pos = i+j*mapWidth;
      
      /*if ( beaches
        ((mapGrid[pos] > oceanThreshold) && (
        ((i>0) && mapGrid[pos-1] < oceanThreshold) ||
        ((i<mapHeight-1) && mapGrid[pos+1] < oceanThreshold) ||
        ((j>0) && mapGrid[pos-mapWidth] < oceanThreshold) ||
        ((j<mapHeight-1) && mapGrid[pos+mapWidth] < oceanThreshold)))) {
        world.colorGrid[pos][0]=1.0f;
        world.colorGrid[pos][1]=0.98f;
        world.colorGrid[pos][2]=0.92f;
        continue;
      }*/
      
      if (mapGrid[pos] < oceanThreshold) {
        //ocean
        world.colorGrid[pos][0]=0.1f;
        world.colorGrid[pos][1]=0.2f;
        world.colorGrid[pos][2]=0.95f;
        continue;
        //mountain
      } 
      if (mapGrid[pos] > mountainThreshold) {
        world.colorGrid[pos][0]=0.3f;
        world.colorGrid[pos][1]=0.35f;
        world.colorGrid[pos][2]=0.3f;
        continue;
      }
      
      //normal land
      world.colorGrid[pos][0]=0.1f;
      world.colorGrid[pos][1]=0.7f;
      world.colorGrid[pos][2]=0.1;
    }
  }
  
  
  
  return world;
}