#include <SFML/Graphics.hpp>
#include <time.h>
#include "Connector.hpp"
using namespace sf;

int size = 56;
// 定義變數size，56 剛好是 Spirit 的大小
Vector2f offset(28,28);
// 圖片偏移量

Sprite f[32]; 
// 定義了一個 Sprite 型別的陣列 f，長度為 32，用於儲存棋子狀態
std::string position="";

int board[8][8] = 
// 第一到八列的棋子數值
    {-1,-2,-3,-4,-5,-3,-2,-1,
     -6,-6,-6,-6,-6,-6,-6,-6,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      6, 6, 6, 6, 6, 6, 6, 6,
      1, 2, 3, 4, 5, 3, 2, 1};

std::string toChessNote(Vector2f p)
{
  std::string s = "";
  s += char(p.x/size+97);
  s += char(7-p.y/size+49);
  return s;
}

Vector2f toCoord(char a,char b)// 將棋譜座標轉換成螢幕上實際的座標（Spirit 座標）
{
   int x = int(a) - 97;
   int y = 7-int(b)+49;
   return Vector2f(x*size,y*size);
}

void move(std::string str)// 移動棋子
{
    Vector2f oldPos = toCoord(str[0],str[1]);
    Vector2f newPos = toCoord(str[2],str[3]);

    for(int i=0;i<32;i++) // 移動後該格子上有棋子則直接覆蓋數據，並將原先的棋子設置在視窗外
     if (f[i].getPosition()==newPos) f[i].setPosition(-100,-100);
        
    for(int i=0;i<32;i++) // 移動後該位置上沒有棋子，則將移動的棋子的數據覆蓋在該格上
     if (f[i].getPosition()==oldPos) f[i].setPosition(newPos);

    // 王車易位
    if (str=="e1g1") if (position.find("e1")==-1) move("h1f1"); 
    if (str=="e8g8") if (position.find("e8")==-1) move("h8f8");
    if (str=="e1c1") if (position.find("e1")==-1) move("a1d1");
    if (str=="e8c8") if (position.find("e8")==-1) move("a8d8");
}

void loadPosition()
{
    int k=0; // 初始化計數器 k （陣列 f 索引）為 0
    
     // 遍歷整個棋盤
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
        {
            // 獲取棋盤位置 (i,j) 的棋子類別
            int n = board[i][j];

            // 如果棋子數值為空（0）則跳過此次迴圈
            if (!n)
                continue;

            // 計算棋子的索引值
            int x = abs(n) - 1;

            // 計算棋子的顏色
            // 負值為白方
            // 正值為黑方
            int y = n > 0 ? 1 : 0;

            // 使用 Spirit 的 setTextureRect 函數設置棋子的紋理矩形位置大小
            f[k].setTextureRect(IntRect(size * x, size * y, size, size));

            // 使用 Spirit 的 setPosition 函數設置棋子在棋盤上的位置
            f[k].setPosition(size * j, size * i);

            // 增加計數器 k 的值
            k++;
        }

    for(int i=0;i<position.length();i+=5)
      move(position.substr(i,4));
}


int main()
{
    RenderWindow window(VideoMode(504, 504), "The Chess! (press SPACE)"); // 設定遊戲視窗的大小

    ConnectToEngine("stockfish.exe");    // 連接西洋棋引擎（定義在 Connector.hpp）

    Texture t1,t2;// 初始化棋子紋理
    t1.loadFromFile("images/figures.png"); // 載入棋子圖片
    t2.loadFromFile("images/board.png");// 載入棋盤圖片

    for(int i=0;i<32;i++) f[i].setTexture(t1);// 將 f 陣列中的每個 Sprite 物件設置為 t1 的紋理
    Sprite sBoard(t2); // 定義了一個名為 sboard 的 Sprite 物件，並用 t2 紋理來初始化它

    loadPosition();// 呼叫 loadPosition 函數，載入棋盤上的棋子的位置和相關圖形

    bool isMove=false;// 將可移動的變數起始值設為 false
    float dx=0, dy=0;
    Vector2f oldPos,newPos;
    std::string str;
    int n=0; 

    while (window.isOpen())
    {
        Vector2i pos = Mouse::getPosition(window) - Vector2i(offset);// 取得滑鼠在視窗中的位置

        Event e; // 初始化事件為變數 e
        while (window.pollEvent(e))// 當遊戲產生事件時
        {
            if (e.type == Event::Closed)// 如果遊戲關閉被點擊則關閉視窗
                window.close();

            //// 退回上一步 ////
            if (e.type == Event::KeyPressed)
                if (e.key.code == Keyboard::BackSpace) // 如果按下BackSpace
                { if (position.length()>6) position.erase(position.length()-6,5); loadPosition();}

             ///// 滑鼠移動旗子 /////
            if (e.type == Event::MouseButtonPressed)
                if (e.key.code == Mouse::Left)
                  for(int i=0;i<32;i++)
                  if (f[i].getGlobalBounds().contains(pos.x,pos.y))// 如果棋子 Spirit 四周邊界包含了滑鼠座標
                      {
                       isMove=true; n=i;// 設置 isMove 為真，令棋子變得可移動
                       dx=pos.x - f[i].getPosition().x;// 計算滑鼠位置和物體 f[i] 的位置得差值作為 dx
                       dy=pos.y - f[i].getPosition().y;// 計算滑鼠位置和物體 f[i] 的位置得差值作為 dy
                       oldPos  =  f[i].getPosition(); // 設定物體的原位置
                      }

             if (e.type == Event::MouseButtonReleased)
                if (e.key.code == Mouse::Left)
                 {
                  isMove=false;// 設置isMove為假
                  Vector2f p = f[n].getPosition() + Vector2f(size/2,size/2);// 計算釋放的位置在哪裡，並轉換成實際座標
                  newPos = Vector2f( size*int(p.x/size), size*int(p.y/size) );// 將旗子擺放至新的位置
                  str = toChessNote(oldPos)+toChessNote(newPos); // 將這次的移動製作成新的移動字串
                  move(str); // 移動棋子
                  if (oldPos!=newPos) position+=str+" "; // 如果棋子有移動，寫入 position 字串
                  f[n].setPosition(newPos);    // 設置 Spirit 座標               
                 }                       
        }

       // 讓 Stockfish 幫你動下一步
       if (Keyboard::isKeyPressed(Keyboard::Space))
       {
         str =  getNextMove(position);
                   
         oldPos = toCoord(str[0],str[1]);
         newPos = toCoord(str[2],str[3]);
         
         for(int i=0;i<32;i++) if (f[i].getPosition()==oldPos) n=i;
         
         ///// 棋子移動動畫 ///////
         for(int k=0;k<50;k++)
          {
            Vector2f p = newPos - oldPos;
            f[n].move(p.x/50, p.y/50); 
            window.draw(sBoard);
            for(int i=0;i<32;i++) f[i].move(offset);
            for(int i=0;i<32;i++) window.draw(f[i]); window.draw(f[n]);
            for(int i=0;i<32;i++) f[i].move(-offset);
            window.display();
          }

        move(str); // 移動棋子
        position+=str+" ";
        f[n].setPosition(newPos); 
        }

        if (isMove) f[n].setPosition(pos.x-dx,pos.y-dy);// 如果isMove為真 則將 Spirit f[n] 的位置設為滑鼠位置減去偏移量 (dx 和 dy)

    ////// draw  ///////
    window.clear();// 進行重製
    window.draw(sBoard);// 繪製棋盤
    for(int i=0;i<32;i++) f[i].move(offset);// 調整棋子偏移位置
    for(int i=0;i<32;i++) window.draw(f[i]); window.draw(f[n]);// 將 f 陣列中的 32 個 Sprite 物件分別繪製到視窗上
    for(int i=0;i<32;i++) f[i].move(-offset);   // 調整棋子偏移位置
    window.display(); // 顯示繪製完成的作品
    }

    CloseConnection(); // 關閉與西洋棋引擎的連結

    return 0;
}
