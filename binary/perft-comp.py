treestumpResults = []
stockfishResults = []

print("Input Treestump results:")

while(True):
  moveLine = input()

  if(moveLine):
    moveLineMove  = moveLine.split(":")[0]
    moveLineNodes = int(moveLine.split(":")[1])
    treestumpResults.append([moveLineMove, moveLineNodes])
  else:
    break

print("Input Stockfish results:")

while(True):
  moveLine = input()

  if(moveLine):
    moveLineMove  = moveLine.split(":")[0]
    moveLineNodes = int(moveLine.split(":")[1])
    stockfishResults.append([moveLineMove, moveLineNodes])
  else:
    break

moveResultsTable = []

for stockfishResult in stockfishResults:
  stockfishInTreestump = False

  for treestumpResult in treestumpResults:
    if(stockfishResult[0] == treestumpResult[0]):
      stockfishInTreestump = True
      moveResultsTable.append([stockfishResult[0], stockfishResult[1], treestumpResult[1]])
      break;

  if(stockfishInTreestump == False):
    moveResultsTable.append([stockfishResult[0], stockfishResult[1], None])


for treestumpResult in treestumpResults:
  treestumpInStockfish = False

  for stockfishResult in stockfishResults:
    if(treestumpResult[0] == stockfishResult[0]):
      treestumpInStockfish = True
      break;

  if(treestumpInStockfish == False):
    moveResultsTable.append([treestumpResult[0], None, treestumpResult[1]])


print("Move    StockF  TreeS   Equal")

for moveResult in sorted(moveResultsTable):
  moveString = moveResult[0]

  stockfishNodes = moveResult[1] if (moveResult[1] != None) else "-"
  treestumpNodes = moveResult[2] if (moveResult[2] != None) else "-"

  equalString = "yes" if (stockfishNodes == treestumpNodes) else "no"

  print("%s\t%s\t%s\t%s" % (moveString, stockfishNodes, treestumpNodes, equalString))