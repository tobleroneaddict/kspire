{
  description = "Space Sandbox";
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = (import nixpkgs) { inherit system; };
        # packages = with pkgs; [
        #   SDL
        #   zlib
        # ];
      in
      {
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "kspire";
          version = "0.1";
          src = ./.;
          nativeBuildInputs = with pkgs; [
            SDL
            zlib
          ];
          buildInputs = with pkgs; [ SDL ];
          buildPhase = ''
            make linux
          '';
          installPhase = ''
            mkdir -p $out/bin
            ls *
            false
          '';
          meta = with pkgs.lib; {
            description = "Space Sandbox";
            license = licenses.mit;
            platforms = platforms.unix;
          };
        };
        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            gcc
            gnumake
            SDL
            zlib
            bear
          ];
          shellHook = ''
            export LD_LIBRARY_PATH=${pkgs.sdl2-compat}/lib:$LD_LIBRARY_PATH
            export PKG_CONFIG_PATH=${pkgs.sdl2-compat}/lib:$PKG_CONFIG_PATH
            pkg-config --cflags --libs sdl2
          '';
        };
      }
    );
}
