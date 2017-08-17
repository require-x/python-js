declare module '@require-x/python' {
  export class Tuple extends Array { }
  export class Kwargs extends Object { }
  export function require(module: string): { [x: string]: any };
  export function kwargs(obj: { [x: string]: any }): Kwargs;
}