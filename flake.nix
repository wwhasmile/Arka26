{
  description = "My lil Breakout clone with blowjob and men.";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, ... }@inputs:
    let
      system = "x86_64-linux";

      pkgs = import inputs.nixpkgs {
        inherit system;
      };
    in {
      devShells.${system}.default = pkgs.mkShellNoCC {
        packages = with pkgs; [
          python3
          compiledb
          clang-tools
          gnumake
          gcc
          gdb
          emscripten
          glibc
          libX11
          libXcursor
          libXrandr
          libXext
          libXi
          libGL
          libGLU
          nixd
        ];
      };
    };
}
