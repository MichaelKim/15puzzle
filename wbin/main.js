function vec(arr) {
  const vec = new Module.VecInt();
  vec.resize(arr.length, 0);
  for (let i = 0; i < arr.length; i++) {
    vec.set(i, arr[i]);
  }
  return vec;
}

function vec2(arr2D) {
  const vec2D = new Module.Vec2Int();
  for (let i = 0; i < arr2D.length; i++) {
    vec2D.push_back(vec(arr2D[i]));
  }
  return vec2D;
}

function vec3(arr3D) {
  const vec3D = new Module.Vec3Int();
  for (let i = 0; i < arr3D.length; i++) {
    vec3D.push_back(vec2(arr3D[i]));
  }
  return vec3D;
}

function arr(vec) {
  const arr = [];
  const size = vec.size();
  for (let i = 0; i < size; i++) {
    arr.push(vec.get(i));
  }
  return arr;
}

function toMoves(sol) {
  const MOVES = ["Null", "N", "E", "W", "S"];
  return sol.map(m => MOVES[m]);
}

Module.addOnPostRun(() => {
  // const database555Reg = vec3([
  //   [[1, 2, 3, 0], [5, 6, 0, 0], [0, 0, 0, 0], [0, 0, 0, 0]],
  //   [[0, 0, 0, 4], [0, 0, 7, 8], [0, 0, 11, 12], [0, 0, 0, 0]],
  //   [[0, 0, 0, 0], [0, 0, 0, 0], [9, 10, 0, 0], [13, 14, 15, 0]]
  // ]);
  //
  // const boards15reg = vec3([
  //   [[1, 2, 3, 4], [5, 6, 8, 12], [9, 10, 7, 0], [13, 14, 11, 15]]
  // ]);

  document.getElementById("solve-btn").addEventListener("click", () => {
    const chunked = [];
    for (let i = 0, y = 0; y < board.height; y++, i += board.width) {
      chunked.push(board.nums.slice(i, i + board.width));
    }

    console.log(board.nums);

    const database8reg = vec3([[[1, 2, 3], [4, 5, 6], [7, 8, 0]]]);
    const boards8reg = vec3([chunked]);

    const solution = arr(Module.solve(database8reg, boards8reg));

    console.log(toMoves(solution));
  });
});

function Tile(x, y, num) {
  const el = document.createElement("div");
  el.className = "tile";
  el.innerHTML = num;
  el.style.gridColumnStart = x;
  el.style.gridRowStart = y;

  this.setPosition = (newX, newY) => {
    el.style.gridColumnStart = newX;
    el.style.gridRowStart = newY;
  };

  this.onClick = callback => {
    el.addEventListener("click", callback);
  };

  this.attach = box => {
    box.appendChild(el);
  };
}

function Board(width, height, direction) {
  const blank = {
    x: direction ? width : 1,
    y: direction ? height : 1
  };
  const length = width * height;

  this.width = width;
  this.height = height;
  this.nums = direction
    ? [...Array(length - 1).keys()].map(i => i + 1).concat(0)
    : [...Array(length).keys()];

  const tiles = this.nums.filter(Boolean).map((num, i) => {
    const index = direction ? i : i + 1;
    // 1 indexed
    let x = (index % width) + 1;
    let y = 0 | (index / width + 1);

    const tile = new Tile(x, y, num);

    tile.onClick(() => {
      const currIndex = (y - 1) * width + (x - 1);
      if (blank.x - 1 === x && blank.y === y) {
        x = blank.x;
        y = blank.y;
        tile.setPosition(x, y);
        this.nums[currIndex] = 0;
        this.nums[currIndex + 1] = num;
        blank.x -= 1;
      } else if (blank.x + 1 === x && blank.y === y) {
        x = blank.x;
        y = blank.y;
        tile.setPosition(x, y);
        this.nums[currIndex] = 0;
        this.nums[currIndex - 1] = num;
        blank.x += 1;
      } else if (blank.y - 1 === y && blank.x === x) {
        x = blank.x;
        y = blank.y;
        tile.setPosition(x, y);
        this.nums[currIndex] = 0;
        this.nums[currIndex + width] = num;
        blank.y -= 1;
      } else if (blank.y + 1 === y && blank.x === x) {
        x = blank.x;
        y = blank.y;
        tile.setPosition(x, y);
        this.nums[currIndex] = 0;
        this.nums[currIndex - width] = num;
        blank.y += 1;
      }
    });

    return tile;
  });

  this.attach = box => {
    box.innerHTML = "";
    tiles.forEach(tile => tile.attach(box));
  };

  this.moveNorth = () => {
    if (blank.y > 0) {
    }
  };
}

const board = new Board(3, 3, true);
board.attach(document.getElementById("root"));
