mkdir Windows\out\
mkdir Windows\out\bin\
mkdir Windows\out\Resources\
copy /y Libraries\bin\lib\* Windows\out\bin\
xcopy /y /E Resources\* Windows\out\Resources\