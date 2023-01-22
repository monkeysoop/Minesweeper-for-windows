# Minesweeper-for-windows
 - gameplay: 
    - left click   -  uncover
    - right click  -  place / remove flag
    - q / Q        -  close and exit
    
 - The game automatically ends in a loss if you click on a mine or you place an incorrect flag and try to uncover next to it.
 - You win if you find and flagg all mines correctly.
## How to use

- Navigate to the src folder and paste this powershell command (it requires gcc to be installed):
```powershell
gcc .\gui.c .\readBMP.c .\gameEngine.c -lgdi32
```
- After it compiles succesfuly you get an executable which you can just run or can give it parameters:
    - either limit just the size and have a default 20% bomb percentile
      ```
      .\a.exe [Width, min 4] [Height, min 4]
      ```
    - or you can specify the bomb precentile too:
      ```
      .\a.exe [Width, min 4] [Height, min 4] [Bomb percentile, max 90%]
      ```
