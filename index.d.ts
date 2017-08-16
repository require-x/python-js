declare module '@require-x/python' {
  export function require(module: string): { [x: string]: any };
  export class Tuple extends Array { }
}